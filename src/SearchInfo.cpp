#include "SearchInfo.hpp"
#include "SearchControl.hpp"

void SearchInfo::clear() {
    fromSearchStart = {};

    nodes = 0;
    nodesQuota = 0;
    currmovenumber = 0;
    _bestmove = {};
    _v = {0, 0, 0, 0, 0};
}

//callbacks from search thread
bool SearchInfo::refreshQuota(const SearchControl& searchControl) {
    static_assert (TickLimit > 0, "TickLimit should be positive number");

    assert (nodesQuota < 0 || nodes >= static_cast<decltype(nodes)>(nodesQuota));
    nodes -= nodesQuota;

    assert (nodesLimit >= nodes);
    auto nodesRemaining = nodesLimit - nodes;

    if (nodesRemaining >= TickLimit) {
        nodesQuota = static_cast<decltype(nodesQuota)>(TickLimit);
    }
    else {
        nodesQuota = static_cast<decltype(nodesQuota)>(nodesRemaining);
        if (nodesRemaining == 0) {
            return true;
        }
    }

    if (searchControl.isStopped()) {
        nodesQuota = 0;
        nodesLimit = nodes;
        return true;
    }

    this->readyok();

    assert (nodesQuota > 0);
    nodes += static_cast<decltype(nodes)>(nodesQuota);

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
