#include "SearchControl.hpp"

#include "Node.hpp"
#include "PositionFen.hpp"
#include "SearchLimit.hpp"
#include "SearchInfo.hpp"
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

    out->write_info_current(info);
}

void SearchControl::report_perft_divide(node_quota_t& quota, Move currmove, index_t currmovenumber, node_count_t perft) {
    releaseNodesQuota(quota);

    info.currmove = currmove;
    info.currmovenumber = currmovenumber;
    info.perft = perft;
    out->report_perft_divide(info);
}

void SearchControl::report_perft_depth(node_quota_t& quota, depth_t depth, node_count_t perft) {
    releaseNodesQuota(quota);

    info.depth = depth;
    info.perft = perft;
    out->report_perft_depth(info);

    clear();

    if (depthLimit > 0 && depth >= depthLimit) {
        searchThread.commandStop();
    }
}

void SearchControl::report_bestmove(node_quota_t& quota, Move bestmove) {
    releaseNodesQuota(quota);

    info.bestmove = bestmove;
    out->report_bestmove(info);

    clear();
    delete root;
}

void SearchControl::go(SearchOutput& output, const Position& startPosition, const SearchLimit& searchLimit) {
    out = &output;

    clear();

    depthLimit = searchLimit.depth;
    nodeLimit = searchLimit.nodes;

    root = (searchLimit.divide)
        ? static_cast<Node*>(new NodePerftDivideRoot(depthLimit))
        : static_cast<Node*>(new NodePerftRoot(depthLimit))
    ;

    searchThread.start(*root, startPosition);
    moveTimer.start(searchThread, searchLimit.getThinkingTime() );
}
