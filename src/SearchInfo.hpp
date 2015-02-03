#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "Move.hpp"

class SearchOutput;
class SearchThread;

struct SearchInfo {
    enum { TickLimit = 1000 }; // ~0.2 msecs
    node_quota_t nodesRemaining; //number of remaining nodes before checking for terminate

    node_count_t nodes;
    node_count_t perftNodes;
    node_count_t nodeLimit; //search limit

    Move    bestmove;
    Move    currmove;
    index_t currmovenumber;

    depth_t depth; //current search depth

    SearchOutput* out;
    Clock clock;

    void clear();
    void releaseNodesQuota();

    bool checkQuota(SearchThread&);
    void acquireNodesQuota(SearchThread&);

    void report_perft_divide();
    void report_perft_depth();
    void report_bestmove();
};

#endif
