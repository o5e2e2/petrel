#ifndef SEARCH_LIMIT_HPP
#define SEARCH_LIMIT_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"

struct SearchLimit {
    bool ponder;
    duration_t time;
    duration_t op_time;
    duration_t inc;
    duration_t op_inc;
    index_t movestogo;
    Ply depth;
    node_count_t nodes;
    index_t mate;
    duration_t movetime;
    bool infinite;

    bool perft;
    bool divide;

    MatrixPiBb searchmoves;

public:
    void clear();
    std::istream& read(std::istream& command, const Position& pos, Color color);
    duration_t getThinkingTime() const;

};

#endif
