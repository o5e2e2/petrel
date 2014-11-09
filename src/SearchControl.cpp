#include "SearchControl.hpp"

#include "Node.hpp"
#include "PositionFen.hpp"
#include "SearchLimit.hpp"
#include "OutputSearch.hpp"

SearchControl::SearchControl ()
    : out{nullptr}, moveTimer{}
{
    clear();
}

void SearchControl::clear() {
    transpositionTable.clear();
    totalNodes = 0;
    clock.restart();
}

void SearchControl::releaseNodesQuota(node_quota_t& quota) {
    totalNodes -= quota;
    quota = 0;
}

void SearchControl::acquireNodesQuota(node_quota_t& quota) {
    totalNodes -= quota;

    if (searchThread.isStopped()) {
        quota = 0;
        return;
    }

    auto remaining = nodeLimit - totalNodes;
    auto q = std::max(remaining, decltype(remaining){0});
    quota = static_cast<node_quota_t>( std::min(q, decltype(q){TickLimit}) );

    if (quota == 0) {
        searchThread.commandStop();
    }

    out->write_info_current();

    totalNodes += quota;
}

void SearchControl::getSearchInfo(SearchInfo& info) const {
    info.nodes = this->totalNodes;
    info.duration = this->clock.read();
}

void SearchControl::report_perft(Move move, index_t currmovenumber, node_count_t perftNodes) const {
    out->report_perft(move, currmovenumber, perftNodes);
}

void SearchControl::report_perft_depth(depth_t depth, node_count_t perftNodes) {
    out->report_perft_depth(depth, perftNodes);

    clear();

    if (depthLimit > 0 && depth >= depthLimit) {
        searchThread.commandStop();
    }
}

void SearchControl::report_bestmove(Move move) {
    out->report_bestmove(move);

    clear();
    delete root;
}

void SearchControl::go(OutputSearch& output, const Position& start_position, const SearchLimit& searchLimit) {
    out = &output;

    clear();

    depthLimit = std::min(searchLimit.depth, MaxDepth);
    nodeLimit = (searchLimit.nodes > 0)? searchLimit.nodes : std::numeric_limits<node_count_t>::max();

    root = (searchLimit.divide)
        ? static_cast<Node*>(new NodePerftDivideRoot(depthLimit))
        : static_cast<Node*>(new NodePerftRoot(depthLimit))
    ;

    moveTimer.cancel(); //cancel the timer from previous go if any
    searchThread.start(*root, start_position);
    moveTimer.start(searchThread, searchLimit.getThinkingTime() );
}
