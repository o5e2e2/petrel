#include "SearchInfo.hpp"
#include "SearchOutput.hpp"
#include "SearchThread.hpp"

void SearchInfo::clear() {
    clock.restart();

    nodes = 0;
    perftNodes = 0;
    perftDivide = 0;
    nodesQuota = 0;
    currmovenumber = 0;
    bestmove = Move::null();
}

void SearchInfo::resetNodesQuota() {
    nodes -= nodesQuota;
    nodesQuota = 0;
}

void SearchInfo::acquireNodesQuota() {
    auto remaining = nodesLimit - nodes;
    if (remaining > 0) {
        nodesQuota = static_cast<decltype(nodesQuota)>( std::min(decltype(remaining){TickLimit}, remaining) );
        nodes += nodesQuota;
    }
}

//callbacks from search thread
bool SearchInfo::checkQuota(SearchThread& searchThread) {
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

void SearchInfo::report_perft_depth() {
    resetNodesQuota();

    out->perft_depth(*this);
    clear();
}

void SearchInfo::report_perft_divide() {
    resetNodesQuota();

    currmovenumber++;
    out->perft_move(*this);
    perftDivide = perftNodes;
}
