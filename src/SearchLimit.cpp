#include "SearchLimit.hpp"

SearchLimit::SearchLimit () :
    time(duration_t::zero()),
    op_time(duration_t::zero()),
    inc(duration_t::zero()),
    op_inc(duration_t::zero()),
    movetime(duration_t::zero()),

    nodes(0),
    movestogo(0),
    depth(0),
    mate(0),

    ponder(false),
    infinite(false),
    perft(false),
    divide(false),

    searchmoves()
{}



duration_t SearchLimit::getThinkingTime() const {
    if (movetime != duration_t::zero()) { return movetime; }

    auto moves_to_go = movestogo? movestogo:60;
    auto average = (time + moves_to_go*inc) / moves_to_go;

    return std::min(time, average);
}
