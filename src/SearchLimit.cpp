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

SearchLimit::SearchLimit (PositionMoves& pm) :
    searchMoves(pm)
{
    clear();
}

void SearchLimit::clear() {
    movetime = time[My] = time[Op] = inc[My] = inc[Op] = Clock::_t::zero();
    ponder = infinite = perft = divide = false;
    nodes = std::numeric_limits<node_count_t>::max();

    movestogo = 0;
    depth = 0;
    mate = 0;
}

const Position& SearchLimit::getPos() const { return searchMoves.getPos(); }

void SearchLimit::read(std::istream& command, Color colorToMove) {
    Side white(colorToMove.is(White)? My : Op);
    Side black(colorToMove.is(Black)? My : Op);

    clear();
    searchMoves.generateMoves();
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
        else if (io::next(command, "searchmoves")) { searchMoves.limitMoves(command, colorToMove); }
        else { break; }
    }

}

Clock::_t SearchLimit::getThinkingTime() const {
    if (movetime != Clock::_t::zero()) { return movetime; }

    auto moves_to_go = movestogo? movestogo : 40;
    auto average = (time[My] + moves_to_go*inc[My]) / moves_to_go;

    return std::min(time[My], average);
}
