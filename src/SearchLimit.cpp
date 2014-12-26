#include "SearchLimit.hpp"
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
    depth(MaxDepth),
    mate(0),

    ponder(false),
    infinite(false),
    perft(false),
    divide(false),

    searchmoves()
{}

void SearchLimit::read(std::istream& command, const Position& startPosition, color_t colorToMove) {
    clear();
    perft = true; //DEBUG

    PositionMoves p(startPosition);
    searchmoves = p.getMoves();

    while (command) {
        if      (io::next(command, "depth"))    { command >> depth; depth = std::min(depth, MaxDepth); }
        else if (io::next(command, "wtime"))    { command >> ((colorToMove == White)? time:op_time); }
        else if (io::next(command, "btime"))    { command >> ((colorToMove == Black)? time:op_time); }
        else if (io::next(command, "winc"))     { command >> ((colorToMove == White)? inc:op_inc); }
        else if (io::next(command, "binc"))     { command >> ((colorToMove == Black)? inc:op_inc); }
        else if (io::next(command, "movestogo")){ command >> movestogo; }
        else if (io::next(command, "nodes"))    { command >> nodes; }
        else if (io::next(command, "movetime")) { command >> movetime; }
        else if (io::next(command, "ponder"))   { ponder = true; }
        else if (io::next(command, "infinite")) { infinite = true; }
        else if (io::next(command, "searchmoves")) { p.limitMoves(command, searchmoves, colorToMove); }
        else if (io::next(command, "perft"))    { perft = true; }
        else if (io::next(command, "divide"))   { divide = true; }
        else { break; }
    }
}

duration_t SearchLimit::getThinkingTime() const {
    if (movetime != duration_t::zero()) { return movetime; }

    auto moves_to_go = movestogo? movestogo:60;
    auto average = (time + moves_to_go*inc) / moves_to_go;

    return std::min(time, average);
}
