#include "SearchControl.hpp"

#include "PositionFen.hpp"
#include "SearchLimit.hpp"
#include "Uci.hpp"

SearchControl::SearchControl ()
    : uci{nullptr},
    moveTimer{}
{
    clear();
}

void SearchControl::clear() {
    transpositionTable.clear();
    totalNodes = 0;
    clock.restart();
}

void SearchControl::releaseNodesQuota(Node::quota_t& quota) {
    totalNodes -= quota;
    quota = 0;
}

void SearchControl::acquireNodesQuota(Node::quota_t& quota) {
    totalNodes -= quota;

    if (searchThread.isStopped()) {
        quota = 0;
        return;
    }

    auto remaining = nodeLimit - totalNodes;
    auto q = std::max(remaining, decltype(remaining){0});
    quota = static_cast<Node::quota_t>( std::min(q, decltype(q){TickLimit}) );

    if (quota == 0) {
        searchThread.commandStop();
    }

    if (ready_ping) {
        ready_ping = false;
        info_current();
    }

    totalNodes += quota;
}

void SearchControl::info_current() const {
    if (uci) {
        uci->write_info_current();
    }
}

void SearchControl::report_perft(Move move, index_t currmovenumber, node_count_t perftNodes) const {
    if (uci) {
        uci->write_perft_move(move, currmovenumber, perftNodes);
    }
}

void SearchControl::report_perft_depth(Ply depth, node_count_t perftNodes) {
    if (uci) {
        uci->write_perft_depth(depth, perftNodes);
    }

    clear();

    if (depthLimit > 0 && depth >= depthLimit) {
        searchThread.commandStop();
    }
}

void SearchControl::report_bestmove(Move move) {
    if (uci) {
        uci->write_bestmove(move);
    }

    clear();
    delete root;
}

bool SearchControl::uci_isready() {
    if (isReady()) {
        return true;
    }
    else {
        ready_ping = true;
        return false;
    }
}

void SearchControl::go(std::istream& in, const Position& start_position, Color colorToMove) {
    ::fail_if_not(isReady(), in);

    SearchLimit searchLimit;
    searchLimit.read(in, start_position, colorToMove);
    if (!in && !in.eof()) {
        return;
    }

    clear();

    depthLimit = std::min(searchLimit.depth, MaxDepth);
    nodeLimit = (searchLimit.nodes > 0)? searchLimit.nodes : std::numeric_limits<node_count_t>::max();

    root = (searchLimit.divide)
        ? static_cast<Node*>(new NodePerftDivideRoot(depthLimit))
        : static_cast<Node*>(new NodePerftRoot(depthLimit))
    ;

    moveTimer.cancel(); //cancel the timer from previous go if any
    searchThread.start(*root, start_position);
    moveTimer.start(searchThread, searchLimit.getThinkingTime() );
}
