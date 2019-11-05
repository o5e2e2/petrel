#ifndef SEARCH_LIMIT_HPP
#define SEARCH_LIMIT_HPP

#include "typedefs.hpp"
#include "Duration.hpp"
#include "PositionFen.hpp"

struct SearchLimit {
    PositionFen positionMoves;

    Side::array<Duration> time = {{ Duration::zero(), Duration::zero() }};
    Side::array<Duration> inc = {{ Duration::zero(), Duration::zero() }};

    Duration movetime = Duration::zero();

    node_count_t nodes = NodeCountMax;
    index_t movestogo = 0;
    depth_t depth = 0;
    index_t mate = 0;

    bool isPonder = false;
    bool isInfinite = false;
    bool isPerft = true;
    bool isDivide = false;
};

#endif
