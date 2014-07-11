#include "SearchControl.hpp"

#include "PositionFen.hpp"
#include "SearchLimit.hpp"
#include "Uci.hpp"
#include "OutputBuffer.hpp"

SearchControl::SearchControl ()
    : uci{nullptr}, moveTimer{}
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

    uci->write_info_current();

    totalNodes += quota;
}

void SearchControl::report_perft(Move move, index_t currmovenumber, node_count_t perftNodes) const {
    uci->report_perft(move, currmovenumber, perftNodes);
}

void SearchControl::report_perft_depth(Ply depth, node_count_t perftNodes) {
    uci->report_perft_depth(depth, perftNodes);

    clear();

    if (depthLimit > 0 && depth >= depthLimit) {
        searchThread.commandStop();
    }
}

void SearchControl::report_bestmove(Move move) {
    uci->report_bestmove(move);

    clear();
    delete root;
}

void SearchControl::go(Uci& _uci, std::istream& in, const Position& start_position, Color colorToMove) {
    if (!isReady()) {
        ::fail_pos(in, 0);
        return;
    }

    uci = &_uci;

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

namespace {
    std::ostream& operator << (std::ostream& out, duration_t duration) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return out << milliseconds;
    }
}

void SearchControl::nps(std::ostream& out) const {
    node_count_t nodes = this->totalNodes;
    duration_t duration = this->clock.read();

    if (nodes > 0) {
        out << " nodes " << nodes;

        if (duration > duration_t::zero()) {
            out << " time " << duration;

            if (duration >= std::chrono::milliseconds{20}) {
                auto _nps = (nodes * duration_t::period::den) / (duration.count() * duration_t::period::num);
                out << " nps " << _nps;
            }
        }
    }
}

void SearchControl::info_nps(std::ostream& out) const {
    if (this->totalNodes > 0) {
        out << "info";
        nps(out);
        out << '\n';
    }
}
