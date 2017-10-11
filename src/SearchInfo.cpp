#include "SearchInfo.hpp"
#include "SearchThread.hpp"

void SearchInfo::clear() {
    clock.restart();

    nodes = 0;
    nodesQuota = 0;
    currmovenumber = 0;
    _bestmove = {};
    _v = {0, 0, 0, 0, 0};
}

//callbacks from search thread
bool SearchInfo::refreshQuota(const SearchThread& searchThread) {
    nodes -= nodesQuota;

    this->readyok();

    if (searchThread.isStopped()) {
        return true;
    }

    if (nodesLimit <= nodes) {
        return true;
    }

    if (nodesLimit - nodes >= TickLimit) {
        nodesQuota = TickLimit;
    }
    else {
        nodesQuota = static_cast<quota_t>(nodesLimit - nodes);
    }

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
