#ifndef SEARCH_LIMIT_HPP
#define SEARCH_LIMIT_HPP

#include "io.hpp"
#include "Index.hpp"
#include "Clock.hpp"
#include "MatrixPiBb.hpp"

class Position;

class SearchLimit {
    static const depth_t MaxDepth = 1000;

    Side::array<duration_t> time;
    Side::array<duration_t> inc;

    duration_t movetime;

    node_count_t nodes;
    index_t movestogo;
    depth_t depth;
    index_t mate;

    bool ponder;
    bool infinite;

    bool perft;
    bool divide;

    MatrixPiBb searchmoves;

    void clear() { *this = {}; }

public:
    SearchLimit ();
    void read(std::istream&, const Position&, color_t);

    depth_t      getDepth()  const { return depth; }
    node_count_t getNodes()  const { return nodes;}
    bool         getDivide() const { return divide; }

    duration_t getThinkingTime() const;
};

#endif
