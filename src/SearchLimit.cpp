#include "SearchLimit.hpp"
#include "PositionFen.hpp"

namespace {
    io::istream& operator >> (io::istream& in, Duration& duration) {
        unsigned long milliseconds;
        if (in >> milliseconds) {
            duration = duration_cast<Duration>(Milliseconds{milliseconds});
        }
        return in;
    }
}

SearchLimit::SearchLimit () {
    clear();
}

void SearchLimit::clear() {
    movetime = time[My] = time[Op] = inc[My] = inc[Op] = Duration::zero();
    ponder = infinite = perft = divide = false;
    nodes = NODE_COUNT_MAX; //almost no limit

    movestogo = 0;
    depth = 0;
    mate = 0;
}

Duration SearchLimit::getThinkingTime() const {
    if (movetime != Duration::zero()) { return movetime; }

    auto moves = movestogo? movestogo : 60;
    auto average = (time[My] + (moves-1)*inc[My]) / moves;

    return std::min(time[My], average);
}

void SearchLimit::readUci(io::istream& command, const PositionFen& positionFen) {
    clear();
    positionMoves = positionFen;

    Color colorToMove = positionFen.getColorToMove();
    Side whiteSide = colorToMove.is(White) ? My : Op;
    Side blackSide = ~whiteSide;

    perft = true; //DEBUG

    using io::next;
    while (command) {
        if      (next(command, "depth"))    { command >> depth; depth = std::min(depth, DEPTH_MAX); }
        else if (next(command, "wtime"))    { command >> time[whiteSide]; }
        else if (next(command, "btime"))    { command >> time[blackSide]; }
        else if (next(command, "winc"))     { command >> inc[whiteSide]; }
        else if (next(command, "binc"))     { command >> inc[blackSide]; }
        else if (next(command, "movestogo")){ command >> movestogo; }
        else if (next(command, "nodes"))    { command >> nodes; nodes = std::min(nodes, NODE_COUNT_MAX); }
        else if (next(command, "movetime")) { command >> movetime; }
        else if (next(command, "mate"))     { command >> mate; }
        else if (next(command, "ponder"))   { ponder = true; }
        else if (next(command, "infinite")) { infinite = true; }
        else if (next(command, "perft"))    { perft = true; }
        else if (next(command, "divide"))   { divide = true; }
        else if (next(command, "searchmoves")) { positionMoves.limitMoves(command); }
        else { break; }
    }
}
