#include "SearchInfo.hpp"
#include "SearchOutput.hpp"
#include "SearchThread.hpp"

void SearchInfo::clear() {
    clock.restart();

    nodes = 0;
    nodesQuota = 0;
    currmovenumber = 0;
    _bestmove = {};
    clearNodes();
}

//callbacks from search thread
bool SearchInfo::checkQuota(const SearchThread& searchThread) {
    if (nodesQuota > 0) {
        return false;
    }

    assert (nodesQuota == 0);

    this->readyok();

    if (searchThread.isStopped()) {
        return true;
    }

    if (nodesLimit >= nodes + TickLimit) {
        nodesQuota = TickLimit;
        nodes += nodesQuota;
        return false;
    }

    if (nodesLimit > nodes) {
        nodesQuota = static_cast<quota_t>(nodesLimit - nodes);
        nodes += nodesQuota;
        return false;
    }

    return true;
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
    _v[PerftDivideNodes] = _v[PerftNodes];

    this->info_currmove();
}
