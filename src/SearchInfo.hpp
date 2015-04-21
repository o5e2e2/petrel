#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "Move.hpp"

class SearchOutput;
class SearchThread;

class SearchInfo {
    enum { TickLimit = 5000 }; // ~1 msec

public:
    node_quota_t nodesQuota; //number of remaining nodes before checking for terminate
    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    SearchOutput* out;
    Clock clock;
    depth_t depth; //current search depth

    Move bestmove;
    Move currmove;
    index_t currmovenumber;

    node_count_t perftNodes;
    node_count_t perftDivide;

private:
    void resetNodesQuota();
    void acquireNodesQuota();

public:
    bool checkQuota(const SearchThread&);

    void clear();
    void report_perft_divide();
    void report_perft_depth();
    void report_bestmove();
};

#endif
