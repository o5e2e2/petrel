#include "SearchInfo.hpp"
#include "SearchThread.hpp"

void SearchInfo::clear() {
    fromSearchStart = {};

    nodes = 0;
    nodesQuota = 0;
    currmovenumber = 0;
    _bestmove = {};
    _v = {0, 0, 0, 0, 0};
}

//callbacks from search thread
bool SearchInfo::refreshQuota(const SearchThread& searchThread) {
    nodes -= nodesQuota;

    auto nodesRemaining = nodesLimit - nodes;
    if (nodesRemaining >= TickLimit) {
        nodesQuota = static_cast<decltype(nodesQuota)>(TickLimit);
    }
    else if (nodesRemaining > 0) {
        nodesQuota = static_cast<decltype(nodesQuota)>(nodesRemaining);
    }
    else {
        assert (nodesRemaining == 0);
        nodesLimit = nodes;
        nodesQuota = 0;
        return true;
    }

    if (searchThread.isStopped()) {
        nodesLimit = nodes;
        nodesQuota = 0;
        return true;
    }

    this->readyok();

    nodes += nodesQuota;

    --nodesQuota; //count current node
    return false;
}

void SearchInfo::report_perft_depth(depth_t draft, node_count_t n) {
    depth = draft;
    _v[PerftNodes] = n;
    this->info_depth();
}

void SearchInfo::report_perft_divide(Move move, index_t moveCount, node_count_t n) {
    currmove = move;
    currmovenumber = moveCount;
    _v[PerftNodes] = n;
    this->info_currmove();
}
