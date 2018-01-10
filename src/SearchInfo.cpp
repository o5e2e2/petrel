#include "SearchInfo.hpp"
#include "SearchThread.hpp"

void SearchInfo::clear() {
    fromSearchStart.restart();

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

void SearchInfo::report_perft_depth(depth_t draft) {
    depth = draft;
    this->info_depth();
    currmovenumber = 0;
    _v[PerftDivideNodes] = 0;
    _v[PerftNodes] = 0;
}

void SearchInfo::report_perft_divide(Move move) {
    currmove = move;
    currmovenumber++;
    this->info_currmove();
    _v[PerftDivideNodes] = _v[PerftNodes];
}
