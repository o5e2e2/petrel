#include "SearchLimit.hpp"
#include "Uci.hpp"
#include "PositionMoves.hpp"

namespace {
    std::istream& operator >> (std::istream& in, duration_t& duration) {
        long milliseconds;
        if (in >> milliseconds) {
            duration = std::chrono::duration_cast<duration_t>(std::chrono::milliseconds{milliseconds});
        }
        return in;
    }
}

SearchLimit::SearchLimit () :
    time(duration_t::zero()),
    op_time(duration_t::zero()),
    inc(duration_t::zero()),
    op_inc(duration_t::zero()),
    movetime(duration_t::zero()),

    nodes(std::numeric_limits<node_count_t>::max()),
    movestogo(0),
    depth(0),
    mate(0),

    ponder(false),
    infinite(false),
    perft(true), //TRICK
    divide(false),

    searchmoves()
{}

void Uci::read_go_limits() {
    limit = {};

    PositionMoves p(start_position);
    limit.searchmoves = p.getMoves();

    while (command) {
        if      (next("depth"))    { command >> limit.depth; }
        else if (next("wtime"))    { command >> ((colorToMove == White)? limit.time:limit.op_time); }
        else if (next("btime"))    { command >> ((colorToMove == Black)? limit.time:limit.op_time); }
        else if (next("winc"))     { command >> ((colorToMove == White)? limit.inc:limit.op_inc); }
        else if (next("binc"))     { command >> ((colorToMove == Black)? limit.inc:limit.op_inc); }
        else if (next("movestogo")){ command >> limit.movestogo; }
        else if (next("nodes"))    { command >> limit.nodes; }
        else if (next("movetime")) { command >> limit.movetime; }
        else if (next("ponder"))   { limit.ponder = true; }
        else if (next("infinite")) { limit.infinite = true; }
        else if (next("searchmoves")) { p.limitMoves(command, limit.searchmoves, colorToMove); }
        else if (next("perft"))    { limit.perft = true; }
        else if (next("divide"))   { limit.divide = true; }
        else { break; }
    }

    limit.depth = (limit.depth > 0)? std::min(limit.depth, static_cast<decltype(limit.depth)>(SearchLimit::MaxDepth)) : 0;
}

duration_t SearchLimit::getThinkingTime() const {
    if (movetime != duration_t::zero()) { return movetime; }

    auto moves_to_go = movestogo? movestogo:60;
    auto average = (time + moves_to_go*inc) / moves_to_go;

    return std::min(time, average);
}
