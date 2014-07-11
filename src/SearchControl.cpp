#include "SearchControl.hpp"

#include "PositionFen.hpp"
#include "SearchLimit.hpp"
#include "Uci.hpp"
#include "OutputBuffer.hpp"

SearchControl::SearchControl ()
    : uci{nullptr}, uci_out{&std::cerr}, moveTimer{}
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
    OutputBuffer out{uci_out};
    out << "info currmovenumber " << currmovenumber << " currmove ";
    uci->write(out, move);
    uci_nps(out);
    out << " string perft " << perftNodes << '\n';
}

void SearchControl::report_perft_depth(Ply depth, node_count_t perftNodes) {
    {
        OutputBuffer out{uci_out};
        out << "info depth " << depth;
        uci_nps(out);
        out << " string perft " << perftNodes << '\n';
    }

    clear();

    if (depthLimit > 0 && depth >= depthLimit) {
        searchThread.commandStop();
    }
}

void SearchControl::report_bestmove(Move move) {
    {
        OutputBuffer out{uci_out};
        uci_info_nps(out);
        out << "bestmove ";
        uci->write(out, move);
        out << '\n';
    }

    clear();
    delete root;
}

void SearchControl::go(Uci& _uci, std::ostream& out, std::istream& in, const Position& start_position, Color colorToMove) {
    ::fail_if_not(isReady(), in);

    uci = &_uci;
    uci_out = &out;

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

void SearchControl::uci_nps(std::ostream& out) const {
    node_count_t nodes = this->totalNodes;
    duration_t duration = this->clock.read();

    if (nodes > 0) {
        out << " nodes " << nodes;

        if (duration > duration_t::zero()) {
            out << " time " << duration;

            auto nps = (nodes * duration_t::period::den) / (duration.count() * duration_t::period::num);
            if (duration >= std::chrono::milliseconds{20}) {
                out << " nps " << nps;
            }
        }
    }
}

void SearchControl::uci_info_nps(std::ostream& out) const {
    if (this->totalNodes > 0) {
        out << "info";
        uci_nps(out);
        out << '\n';
    }
}
