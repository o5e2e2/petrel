#include "SearchInfo.hpp"
#include "SearchOutput.hpp"
#include "SearchThread.hpp"

void SearchInfo::clear() {
    clock.restart();

    nodes = 0;
    perftNodes = 0;
    nodesRemaining = 0;
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

    if (nodes >= nodeLimit) {
        searchThread.commandStop();
    }
    else {
        auto remaining = nodeLimit - nodes;
        nodesRemaining = static_cast<node_quota_t>( std::min(remaining, decltype(remaining){TickLimit}) );
        nodes += nodesRemaining;
    }

    out->report_current(*this);
}

void SearchInfo::report_perft_divide() {
    releaseNodesQuota();
    out->report_perft_divide(*this);
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
