#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "Move.hpp"

struct SearchInfo {
    Clock clock;

    node_count_t nodes;
    node_count_t perftNodes;
    node_quota_t nodesRemaining; //number of remaining nodes before checking for terminate
    Move    bestmove;

    Move    currmove;
    index_t currmovenumber;
    depth_t depth;

    void clear() {
        clock.restart();

        nodes = 0;
        perftNodes = 0;
        nodesRemaining = 0;
        bestmove = Move::null();
    }

    void releaseNodesQuota() {
        nodes -= nodesRemaining;
        nodesRemaining = 0;
    }

};

#endif
