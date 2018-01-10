#ifndef SEARCH_LIMIT_HPP
#define SEARCH_LIMIT_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "UciPosition.hpp"

class SearchLimit {
    UciPosition positionMoves;

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

    void clear();

public:
    SearchLimit ();

    depth_t      getDepth()  const { return depth; }
    node_count_t getNodes()  const { return nodes; }
    bool         getDivide() const { return divide; }

    const UciPosition& getPositionMoves() const { return positionMoves; }

    Duration getThinkingTime() const;

    void readUci(io::istream&, const UciPosition&);
};

#endif
