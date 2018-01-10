#include "SearchLimit.hpp"
#include "UciPosition.hpp"

namespace {
    io::istream& operator >> (io::istream& in, Clock::_t& duration) {
        unsigned long milliseconds;
        if (in >> milliseconds) {
            duration = std::chrono::duration_cast<Clock::_t>(std::chrono::milliseconds{milliseconds});
        }
        return in;
    }
}

SearchLimit::SearchLimit () {
    clear();
}

void SearchLimit::clear() {
    movetime = time[My] = time[Op] = inc[My] = inc[Op] = Clock::_t::zero();
    ponder = infinite = perft = divide = false;
    nodes = NODE_COUNT_MAX; //no limit

    movestogo = 0;
    depth = 0;
    mate = 0;
}

Clock::_t SearchLimit::getThinkingTime() const {
    if (movetime != Clock::_t::zero()) { return movetime; }

    auto moves_to_go = movestogo? movestogo : 60;
    auto average = (time[My] + (moves_to_go-1)*inc[My]) / moves_to_go;

    return std::min(time[My], average);
}

void SearchLimit::readUci(io::istream& command, const UciPosition& uciPosition) {
    positionMoves = uciPosition;

    Color colorToMove = uciPosition.getColorToMove();
    Side white = colorToMove.is(White)? My : Op;
    Side black = colorToMove.is(Black)? My : Op;

    clear();
    perft = true; //DEBUG

    using io::next;
    while (command) {
        if      (next(command, "depth"))    { command >> depth; depth = std::min(depth, DEPTH_MAX); }
        else if (next(command, "wtime"))    { command >> time[white]; }
        else if (next(command, "btime"))    { command >> time[black]; }
        else if (next(command, "winc"))     { command >> inc[white]; }
        else if (next(command, "binc"))     { command >> inc[black]; }
        else if (next(command, "movestogo")){ command >> movestogo; }
        else if (next(command, "nodes"))    { command >> nodes; nodes = std::min(nodes, NODE_COUNT_MAX); }
        else if (next(command, "movetime")) { command >> movetime; }
        else if (next(command, "ponder"))   { ponder = true; }
        else if (next(command, "infinite")) { infinite = true; }
        else if (next(command, "perft"))    { perft = true; }
        else if (next(command, "divide"))   { divide = true; }
        else if (next(command, "searchmoves")) { positionMoves.limitMoves(command); }
        else { break; }
    }
}
