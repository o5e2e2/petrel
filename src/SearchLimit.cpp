#include "SearchLimit.hpp"
#include "UciPosition.hpp"

namespace {
    std::istream& operator >> (std::istream& in, Clock::_t& duration) {
        long milliseconds;
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
    nodes = NODE_COUNT_NONE;

    movestogo = 0;
    depth = 0;
    mate = 0;
}

Clock::_t SearchLimit::getThinkingTime() const {
    if (movetime != Clock::_t::zero()) { return movetime; }

    auto moves_to_go = movestogo? movestogo : 40;
    auto average = (time[My] + moves_to_go*inc[My]) / moves_to_go;

    return std::min(time[My], average);
}

void SearchLimit::readUci(std::istream& command, const UciPosition& uciPosition, UciPosition* resultPosition) {
    *resultPosition = uciPosition;

    Color colorToMove = uciPosition.getColorToMove();
    Side white = colorToMove.is(White)? My : Op;
    Side black = colorToMove.is(Black)? My : Op;

    clear();
    perft = true; //DEBUG

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
        else if (io::next(command, "perft"))    { perft = true; }
        else if (io::next(command, "divide"))   { divide = true; }
        else if (io::next(command, "searchmoves")) { resultPosition->limitMoves(command); }
        else { break; }
    }
}
