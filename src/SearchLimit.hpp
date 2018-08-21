#ifndef SEARCH_LIMIT_HPP
#define SEARCH_LIMIT_HPP

#include "typedefs.hpp"
#include "PositionFen.hpp"

class SearchLimit {
    friend class UciSearchControl;

    PositionFen positionMoves;

    Side::array<Duration> time;
    Side::array<Duration> inc;

    Duration movetime;

    node_count_t nodes;
    index_t movestogo;
    depth_t depth;
    index_t mate;

    bool ponder;
    bool infinite;

    bool perft;
    bool divide;

public:
    SearchLimit ();

    depth_t      getDepth() const { return depth; }
    node_count_t getNodes() const { return nodes; }
    bool         isDivide() const { return divide; }

    const PositionFen& getPositionMoves() const { return positionMoves; }

    Duration getThinkingTime() const;
};

#endif
