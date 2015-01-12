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

void SearchControl::releaseNodesQuota(node_quota_t& quota) {
    info.nodes -= quota;
    quota = 0;
}

void SearchControl::acquireNodesQuota(node_quota_t& quota) {
    releaseNodesQuota(quota);

    if (searchThread.isStopped()) {
        return;
    }

    if (info.nodes >= nodeLimit) {
        searchThread.commandStop();
    }
    else {
        auto remaining = nodeLimit - info.nodes;
        quota = static_cast<node_quota_t>( std::min(remaining, decltype(remaining){TickLimit}) );
        info.nodes += quota;
    }

    out->report_current(info);
}

void SearchControl::report_perft_divide(node_quota_t& quota, Move currmove, index_t currmovenumber) {
    releaseNodesQuota(quota);

    info.currmove = currmove;
    info.currmovenumber = currmovenumber;
    out->report_perft_divide(info);
}

void SearchControl::report_perft_depth(node_quota_t& quota, depth_t depth) {
    releaseNodesQuota(quota);

    info.depth = depth;
    out->report_perft_depth(info);

    clear();

    if (depthLimit > 0 && depth >= depthLimit) {
        searchThread.commandStop();
    }
}

void SearchControl::report_bestmove(node_quota_t& quota) {
    releaseNodesQuota(quota);

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
