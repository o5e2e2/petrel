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

void SearchInfo::releaseNodesQuota() {
    nodes -= nodesQuota;
    nodesQuota = 0;
}

//callbacks from search thread
bool SearchInfo::checkQuota(SearchThread& searchThread) {
    if (nodesQuota <= 0) {
        acquireNodesQuota(searchThread);
    }
    return nodesQuota <= 0;
}

void SearchInfo::acquireNodesQuota(SearchThread& searchThread) {
    releaseNodesQuota();

    if (searchThread.isStopped()) {
        return;
    }

    auto remaining = nodesLimit - nodes;
    if (remaining > 0) {
        nodesQuota = static_cast<decltype(nodesQuota)>( std::min(decltype(remaining){TickLimit}, remaining) );
        nodes += nodesQuota;
    }
    else {
        searchThread.commandStop(); //stop itself
    }

    out->report_current(*this);
}

void SearchInfo::report_perft_divide() {
    releaseNodesQuota();

    currmovenumber++;
    out->report_perft_divide(*this);
    perftDivide = perftNodes;
}

void SearchInfo::report_perft_depth() {
    releaseNodesQuota();
    out->report_perft_depth(*this);
    clear();
}

void SearchInfo::report_bestmove() {
    releaseNodesQuota();
    out->report_bestmove(*this);
    clear();
}
