#include "SearchControl.hpp"

#include "Node.hpp"
#include "SearchLimit.hpp"
#include "SearchOutput.hpp"

SearchControl::SearchControl ()
    : out{nullptr}, moveTimer{}
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

    out->report_current(info);
}

void SearchControl::report_perft_divide() {
    info.releaseNodesQuota();
    out->report_perft_divide(info);
}

void SearchControl::report_perft_depth() {
    info.releaseNodesQuota();

    out->report_perft_depth(info);

    clear();

    if (depthLimit > 0 && info.depth >= depthLimit) {
        searchThread.commandStop();
    }
}

void SearchControl::report_bestmove() {
    info.releaseNodesQuota();
    out->report_bestmove(info);

    clear();
    delete root;
}

void SearchControl::go(SearchOutput& output, const Position& startPosition, const SearchLimit& goLimit) {
    out = &output;

    clear();

    depthLimit = goLimit.getDepth();
    nodeLimit = goLimit.getNodes();

    root = (goLimit.getDivide())
        ? static_cast<Node*>(new NodePerftDivideRoot(*this, depthLimit))
        : static_cast<Node*>(new NodePerftRoot(*this, depthLimit))
    ;

    searchThread.start(*root, startPosition);
    moveTimer.start(searchThread, goLimit.getThinkingTime() );
}
