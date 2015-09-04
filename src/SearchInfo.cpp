#include "SearchInfo.hpp"
#include "SearchOutput.hpp"
#include "SearchThread.hpp"

void SearchInfo::clear() {
    clock.restart();

    nodes = 0;
    nodesQuota = 0;
    currmovenumber = 0;
    bestmove = {};
    clearNodes();
}

void SearchInfo::resetNodesQuota() {
    nodes -= nodesQuota;
    nodesQuota = 0;
}

void SearchInfo::acquireNodesQuota() {
    auto remaining = nodesLimit - nodes;
    if (remaining > 0) {
        auto nextQuota = std::min(remaining, +TickLimit);
        nodesQuota = static_cast<decltype(nodesQuota)>(nextQuota);
        nodes += nodesQuota;
    }
}

//callbacks from search thread
bool SearchInfo::checkQuota(const SearchThread& searchThread) {
    if (nodesQuota <= 0) {
        if (!searchThread.isStopped()) {
            resetNodesQuota();
            acquireNodesQuota();
            out->readyok(*this);
        }
    }
    return nodesQuota <= 0;
}

void SearchInfo::report_bestmove() {
    resetNodesQuota();

    out->bestmove(*this);
    clear();
}

void SearchInfo::report_perft_depth(depth_t draft) {
    resetNodesQuota();

    depth = draft;
    out->info_depth(*this);
    clear();
}

void SearchInfo::report_perft_divide(Move move) {
    resetNodesQuota();

    currmove = move;
    currmovenumber++;
    out->info_currmove(*this);
    _v[PerftDivideNodes] = _v[PerftNodes];
}
