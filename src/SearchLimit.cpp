#include "SearchLimit.hpp"
#include "PositionMoves.hpp"

namespace {
    std::istream& operator >> (std::istream& in, Clock::_t& duration) {
        long milliseconds;
        if (in >> milliseconds) {
            duration = std::chrono::duration_cast<Clock::_t>(std::chrono::milliseconds{milliseconds});
        }
        return in;
    }
}

SearchLimit::SearchLimit () :
    movetime(Clock::_t::zero()),

    nodes(std::numeric_limits<node_count_t>::max()),
    movestogo(0),
    depth(0),
    mate(0),

    ponder(false),
    infinite(false),
    perft(false),
    divide(false),

    searchmoves()
{
    time[My] = time[Op] = inc[My] = inc[Op] = Clock::_t::zero();
}

void SearchLimit::read(std::istream& command, const Position& startPosition, color_t colorToMove) {
    Side white{(colorToMove == White)? My:Op};
    Side black{(colorToMove == Black)? My:Op};

    clear();

    PositionMoves p(startPosition);
    searchmoves = p.getMoves();

    while (command) {
        if      (io::next(command, "depth"))    { command >> depth; depth = std::min(depth, MaxDepth); }
        else if (io::next(command, "wtime"))    { command >> time[white]; }
        else if (io::next(command, "btime"))    { command >> time[black]; }
        else if (io::next(command, "winc"))     { command >> inc[white]; }
        else if (io::next(command, "binc"))     { command >> inc[black]; }
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

    perft = true; //DEBUG
}

Clock::_t SearchLimit::getThinkingTime() const {
    if (movetime != Clock::_t::zero()) { return movetime; }

    auto moves_to_go = movestogo? movestogo:60;
    auto average = (time[My] + moves_to_go*inc[My]) / moves_to_go;

    return std::min(time[My], average);
}
