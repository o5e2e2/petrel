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
    info.nodes = 0;
    info.clock.restart();
}

void SearchControl::releaseNodesQuota(node_quota_t& quota) {
    info.nodes -= quota;
    quota = 0;
}

void SearchControl::acquireNodesQuota(node_quota_t& quota) {
    info.nodes -= quota;

    if (searchThread.isStopped()) {
        quota = 0;
        return;
    }

    auto remaining = nodeLimit - info.nodes;
    auto q = std::max(remaining, decltype(remaining){0});
    quota = static_cast<node_quota_t>( std::min(q, decltype(q){TickLimit}) );

    if (quota == 0) {
        searchThread.commandStop();
    }

    out->write_info_current(info);

    info.nodes += quota;
}

void SearchControl::report_perft(Move move, index_t currmovenumber, node_count_t perftNodes) {
    info.perft = perftNodes;
    info.currmove = move;
    info.currmovenumber = currmovenumber;
    out->report_perft(info);
}

void SearchControl::report_perft_depth(depth_t depth, node_count_t perftNodes) {
    info.perft = perftNodes;
    info.depth = depth;
    out->report_perft_depth(info);

    clear();

    if (depthLimit > 0 && depth >= depthLimit) {
        searchThread.commandStop();
    }
}

void SearchControl::report_bestmove(Move move) {
    info.bestmove = move;
    out->report_bestmove(info);

    clear();
    delete root;
}

void SearchControl::go(SearchOutput& output, const Position& start_position, const SearchLimit& searchLimit) {
    out = &output;

    clear();

    depthLimit = searchLimit.depth;
    nodeLimit = searchLimit.nodes;

    root = (searchLimit.divide)
        ? static_cast<Node*>(new NodePerftDivideRoot(depthLimit))
        : static_cast<Node*>(new NodePerftRoot(depthLimit))
    ;

    searchThread.start(*root, start_position);
    moveTimer.start(searchThread, searchLimit.getThinkingTime() );
}
