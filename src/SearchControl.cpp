#include "SearchControl.hpp"

#include "Node.hpp"
#include "SearchLimit.hpp"
#include "SearchOutput.hpp"

SearchControl::SearchControl ()
    : moveTimer{}
{
    clear();
}

void SearchControl::clear() {
    transpositionTable.clear();
    info.clear();
}

void SearchControl::acquireNodesQuota() {
    info.releaseNodesQuota();

    if (searchThread.isStopped()) {
        return;
    }

    if (info.nodes >= nodeLimit) {
        searchThread.commandStop();
    }
    else {
        auto remaining = nodeLimit - info.nodes;
        info.nodesRemaining = static_cast<node_quota_t>( std::min(remaining, decltype(remaining){TickLimit}) );
        info.nodes += info.nodesRemaining;
    }

    info.out->report_current(info);
}

void SearchControl::report_perft_divide() {
    info.releaseNodesQuota();
    info.out->report_perft_divide(info);
}

void SearchControl::report_perft_depth() {
    info.releaseNodesQuota();

    info.out->report_perft_depth(info);

    clear();
}

void SearchControl::report_bestmove() {
    info.releaseNodesQuota();
    info.out->report_bestmove(info);

    clear();
}

void SearchControl::go(SearchOutput& output, const Position& startPosition, const SearchLimit& goLimit) {
    info.out = &output;

    clear();

    nodeLimit = goLimit.getNodes();

    auto searchFn = goLimit.getDivide()? PerftDivideRoot::perft : PerftRoot::perft;

    searchThread.start(searchFn, this, &startPosition, goLimit.getDepth());
    moveTimer.start(searchThread, goLimit.getThinkingTime() );
}
