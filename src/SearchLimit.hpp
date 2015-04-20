#ifndef SEARCH_LIMIT_HPP
#define SEARCH_LIMIT_HPP

#include "io.hpp"
#include "Index.hpp"
#include "Clock.hpp"

class Position;
class PositionMoves;

class SearchLimit {
    static const depth_t MaxDepth = 1000;

    Side::array<Clock::_t> time;
    Side::array<Clock::_t> inc;

    Clock::_t movetime;

    node_count_t nodes;
    index_t movestogo;
    depth_t depth;
    index_t mate;

    bool ponder;
    bool infinite;

    bool perft;
    bool divide;

    PositionMoves& searchMoves;

    void clear();

public:
    SearchLimit (PositionMoves&);
    void read(std::istream&, Color);

    depth_t      getDepth()  const { return depth; }
    node_count_t getNodes()  const { return nodes;}
    bool         getDivide() const { return divide; }

    const PositionMoves& getSearchMoves() const { return searchMoves; }
    const Position& getPos() const;

    Clock::_t getThinkingTime() const;
};

#endif
