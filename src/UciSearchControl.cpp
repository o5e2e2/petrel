#include "UciSearchControl.hpp"

namespace {
    io::istream& operator >> (io::istream& in, Duration& duration) {
        unsigned long milliseconds;
        if (in >> milliseconds) {
            duration = duration_cast<Duration>(Milliseconds{milliseconds});
        }
        return in;
    }
}

UciSearchControl::UciSearchControl (UciSearchInfo& i) : SearchControl(i) {}

void UciSearchControl::setHash(io::istream& command) {
    if (!this->isReady()) {
        io::fail_rewind(command);
        return;
    }

    size_t quantity = 0;
    command >> quantity;
    if (!command) {
        io::fail_rewind(command);
        return;
    }

    io::char_type unit = 'm';
    command >> unit;

    switch (std::tolower(unit)) {
        case 't': quantity *= 1024;
        case 'g': quantity *= 1024;
        case 'm': quantity *= 1024;
        case 'k': quantity *= 1024;
        case 'b': break;

        default: {
            io::fail_rewind(command);
            return;
        }
    }

    tt().resize(quantity);
}

void UciSearchControl::go(io::istream& command, const PositionFen& positionFen) {
    auto& l = searchLimit;
    l = {};

    Color colorToMove = positionFen.getColorToMove();
    auto whiteSide = colorToMove.is(White) ? My : Op;
    auto blackSide = ~whiteSide;

    l.positionMoves = positionFen;
    l.perft = true; //DEBUG

    using io::next;
    while (command) {
        if      (next(command, "depth"))    { command >> l.depth; l.depth = std::min(l.depth, static_cast<depth_t>(DepthMax)); }
        else if (next(command, "wtime"))    { command >> l.time[whiteSide]; }
        else if (next(command, "btime"))    { command >> l.time[blackSide]; }
        else if (next(command, "winc"))     { command >> l.inc[whiteSide]; }
        else if (next(command, "binc"))     { command >> l.inc[blackSide]; }
        else if (next(command, "movestogo")){ command >> l.movestogo; }
        else if (next(command, "nodes"))    { command >> l.nodes; l.nodes = std::min(l.nodes, static_cast<node_count_t>(NodeCountMax)); }
        else if (next(command, "movetime")) { command >> l.movetime; }
        else if (next(command, "mate"))     { command >> l.mate; }
        else if (next(command, "ponder"))   { l.ponder = true; }
        else if (next(command, "infinite")) { l.infinite = true; }
        else if (next(command, "perft"))    { l.perft = true; }
        else if (next(command, "divide"))   { l.divide = true; }
        else if (next(command, "searchmoves")) { l.positionMoves.limitMoves(command); }
        else { break; }
    }

    SearchControl::go();
}
