#include "SearchInfo.hpp"
#include "SearchOutput.hpp"
#include "SearchThread.hpp"

void SearchInfo::clear() {
    clock.restart();

    nodes = 0;
    perftNodes = 0;
    perftDivide = 0;
    nodesRemaining = 0;
    currmovenumber = 0;
    bestmove = Move::null();
}

void SearchInfo::releaseNodesQuota() {
    nodes -= nodesRemaining;
    nodesRemaining = 0;
}

//callbacks from search thread
bool SearchInfo::checkQuota(SearchThread& searchThread) {
    if (nodesRemaining <= 0) {
        acquireNodesQuota(searchThread);
    }
    return nodesRemaining <= 0;
}

void SearchInfo::acquireNodesQuota(SearchThread& searchThread) {
    releaseNodesQuota();

    if (searchThread.isStopped()) {
        return;
    }

    if (nodeLimit > nodes) {
        auto remaining = small_cast<decltype(nodesRemaining)>(nodeLimit - nodes);
        nodesRemaining = std::min(remaining, decltype(remaining){TickLimit});
        nodes += nodesRemaining;
    }
    else {
        //stop itself if node quota ended
        searchThread.commandStop();
        return;
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
