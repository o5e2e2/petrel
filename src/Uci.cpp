#include "Uci.hpp"
#include "SearchLimit.hpp"

namespace {
    io::istream& operator >> (io::istream& in, Duration& duration) {
        unsigned long milliseconds;
        if (in >> milliseconds) {
            duration = duration_cast<Duration>(Milliseconds{milliseconds});
        }
        return in;
    }
}

Uci::Uci (io::ostream& out):
    positionFen{},
    info(out, positionFen.getColorToMove(), positionFen.getChessVariant()),
    searchControl(info)
{
    ucinewgame();
}

void Uci::operator() (io::istream& in, io::ostream& err) {
    std::string currentLine;
    while (std::getline(in, currentLine)) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(currentLine));
        command >> std::ws;

        if      (next("go"))        { go(); }
        else if (next("position"))  { position(); }
        else if (next("stop"))      { searchControl.stop(); }
        else if (next("isready"))   { isready(); }
        else if (next("setoption")) { setoption(); }
        else if (next("set"))       { setoption(); }
        else if (next("ucinewgame")){ ucinewgame(); }
        else if (next("uci"))       { uciok(); }
        else if (next("quit"))      { break; }
        else if (next("exit"))      { break; }

        //report error if something left unparsed
        if (!nextNone()) {
            err << "parsing error: " << command.rdbuf() << '\n';
        }
    }
}

void Uci::ucinewgame() {
    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    searchControl.newGame();
    positionFen.setStartpos();
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true"))  { positionFen.setChessVariant(Chess960); return; }
        if (next("false")) { positionFen.setChessVariant(Orthodox); return; }

        io::fail_rewind(command);
        return;
    }

    if (next("Hash")) {
        next("value");
        setHash();
        return;
    }

}

void Uci::setHash() {
    if (!searchControl.isReady()) {
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

    searchControl.tt().resize(quantity);
}

void Uci::position() {
    if (nextNone()) {
        info.position(positionFen);
        return;
    }

    if (next("startpos")) { positionFen.setStartpos(); }
    if (next("fen")) { positionFen.readFen(command); }

    next("moves");
    positionFen.playMoves(command);
}

void Uci::go() {
    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    Color colorToMove = positionFen.getColorToMove();
    auto whiteSide = colorToMove.is(White) ? My : Op;
    auto blackSide = ~whiteSide;

    SearchLimit l;
    l.positionMoves = positionFen;

    while (command) {
        if      (next("depth"))    { command >> l.depth; l.depth = std::min(l.depth, static_cast<depth_t>(DepthMax)); }
        else if (next("wtime"))    { command >> l.time[whiteSide]; }
        else if (next("btime"))    { command >> l.time[blackSide]; }
        else if (next("winc"))     { command >> l.inc[whiteSide]; }
        else if (next("binc"))     { command >> l.inc[blackSide]; }
        else if (next("movestogo")){ command >> l.movestogo; }
        else if (next("nodes"))    { command >> l.nodes; l.nodes = std::min(l.nodes, static_cast<node_count_t>(NodeCountMax)); }
        else if (next("movetime")) { command >> l.movetime; }
        else if (next("mate"))     { command >> l.mate; }
        else if (next("ponder"))   { l.ponder = true; }
        else if (next("infinite")) { l.infinite = true; }
        else if (next("perft"))    { l.perft = true; }
        else if (next("divide"))   { l.divide = true; }
        else if (next("searchmoves")) { l.positionMoves.limitMoves(command); }
        else if (!nextNone())      { io::fail_here(command); return; }
        else { break; }
    }

    searchControl.go(l);
}

void Uci::isready() {
    info.isready(searchControl.isReady());
}

void Uci::uciok() {
    searchControl.uciok();
}
