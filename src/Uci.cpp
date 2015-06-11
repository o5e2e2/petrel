#include <fstream>
#include <cstdlib>

#include "Uci.hpp"
#include "PositionFen.hpp"

#define SHOULD_BE_READY  if (!searchControl.isReady()) { io::fail_rewind(command); return; }

namespace {
    std::istream& operator >> (std::istream& in, Clock::_t& duration) {
        long milliseconds;
        if (in >> milliseconds) {
            duration = std::chrono::duration_cast<Clock::_t>(std::chrono::milliseconds{milliseconds});
        }
        return in;
    }
}

Uci::Uci (std::ostream& out):
    searchControl(),
    uciOutput(out, colorToMove, searchControl.tt()),
    searchMoves(rootPosition)
{
    ucinewgame();
}

void Uci::operator() (std::istream& in) {
    for (std::string commandLine; std::getline(in, commandLine); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(commandLine));
        command >> std::ws;

        if      (next("position"))  { position(); }
        else if (next("go"))        { go(); }
        else if (next("stop"))      { searchControl.stop(); }
        else if (next("isready"))   { uciOutput.isready(searchControl); }
        else if (next("setoption")) { setoption(); }
        else if (next("ucinewgame")){ ucinewgame(); }
        else if (next("uci"))       { uciOutput.uciok(); }
        else if (next("quit"))      { break; }
        //UCI extensions
        else if (next("exit"))      { exit(); }
        else if (next("wait"))      { searchControl.wait(); }
        else if (next("echo"))      { echo(); }
        else if (next("call"))      { call(); }
        else {
            //ignore comment line
            auto peek = command.peek();
            if (peek == '#' || peek == ';') { continue; }
        }

        //report error if something not parsed left
        if (!next("")) { uciOutput.error(command); }
    }
}

void Uci::ucinewgame() {
    SHOULD_BE_READY;

    startpos();
    searchControl.clear();
    uciOutput.newGame();
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true"))  { uciOutput.setChess960(true);  return; }
        if (next("false")) { uciOutput.setChess960(false); return; }

        return;
    }

    SHOULD_BE_READY;

    if (next("Hash")) {
        next("value");

        UciHash::_t mebibytes;
        if (command >> mebibytes) { uciOutput.resizeHash(mebibytes); }

        return;
    }

    io::fail_rewind(command);
}

void Uci::position() {
    if (next("")) {
        uciOutput.info_fen(rootPosition);
        return;
    }

    SHOULD_BE_READY;

    if (next("startpos")) { startpos(); }
    if (next("fen")) { PositionFen::read(command, rootPosition, colorToMove); }

    next("moves");

    PositionMoves::makeMoves(command, rootPosition, colorToMove);
}

void Uci::startpos() {
    PositionFen::setStartpos(rootPosition, colorToMove);
}

void Uci::go() {
    SHOULD_BE_READY;

    Side white(colorToMove.is(White)? My : Op);
    Side black(colorToMove.is(Black)? My : Op);

    auto& g = goLimit;

    g.clear();
    searchMoves.generateMoves();
    g.perft = true; //DEBUG

    while (command) {
        if      (next("depth"))    { command >> g.depth; g.depth = std::min(g.depth, g.MaxDepth); }
        else if (next("wtime"))    { command >> g.time[white]; }
        else if (next("btime"))    { command >> g.time[black]; }
        else if (next("winc"))     { command >> g.inc[white]; }
        else if (next("binc"))     { command >> g.inc[black]; }
        else if (next("movestogo")){ command >> g.movestogo; }
        else if (next("nodes"))    { command >> g.nodes; }
        else if (next("movetime")) { command >> g.movetime; }
        else if (next("ponder"))   { g.ponder = true; }
        else if (next("infinite")) { g.infinite = true; }
        else if (next("perft"))    { g.perft = true; }
        else if (next("divide"))   { g.divide = true; }
        else if (next("searchmoves")) { searchMoves.limitMoves(command, colorToMove); }
        else { break; }
    }

    if (next("")) {
        searchControl.go(uciOutput, rootPosition, goLimit);
    }
}

void Uci::echo() const {
    command >> std::ws;
    uciOutput.echo(command);
}

void Uci::exit() const {
    int exit_code = 0;
    command >> exit_code;
    std::exit(exit_code);
}

void Uci::call(const std::string& filename) {
    std::ifstream file(filename);

    if (!file) {
        uciOutput.error(std::string("error opening file: ") + filename);
        return;
    }

    operator()(file);
}

void Uci::call() {
    std::string filename;
    command >> filename;

    if (filename.empty() || !next("")) {
        uciOutput.error(command);
        return;
    }

    call(filename);
}

#undef SHOULD_BE_READY
