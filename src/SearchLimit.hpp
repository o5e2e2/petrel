#ifndef SEARCH_LIMIT_HPP
#define SEARCH_LIMIT_HPP

#include "typedefs.hpp"
#include "PositionFen.hpp"

class SearchLimit {
    friend class Uci;

    PositionFen positionMoves;

    Side::array<Duration> time = {{ Duration::zero(), Duration::zero() }};
    Side::array<Duration> inc = {{ Duration::zero(), Duration::zero() }};

    Duration movetime = Duration::zero();

    node_count_t nodes = NodeCountMax;
    index_t movestogo = 0;
    depth_t depth = 0;
    index_t mate = 0;

    bool ponder = false;
    bool infinite = false;

    bool perft = true;
    bool divide = false;

public:
    depth_t      getDepthLimit() const { return depth; }
    node_count_t getNodesLimit() const { return nodes; }
    bool         isDivide() const { return divide; }

    const PositionFen& getMovesLimit() const { return positionMoves; }

    Duration getThinkingTime() const;
};

#endif
