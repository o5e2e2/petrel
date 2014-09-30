#ifndef SEARCH_LIMIT_HPP
#define SEARCH_LIMIT_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "MatrixPiBb.hpp"

struct SearchLimit {
    duration_t time;
    duration_t op_time;
    duration_t inc;
    duration_t op_inc;
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

public:
    SearchLimit ();
    duration_t getThinkingTime() const;

    friend class Uci;
};

#endif
