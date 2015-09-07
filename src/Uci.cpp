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
    chessVariant(Orthodox),
    searchMoves(rootPosition),
    uciOutput(out, colorToMove, chessVariant, searchControl.tt())
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
        else if (next("set"))       { setoption(); }
        else { //ignore comment line
            auto peek = command.peek();
            if (peek == '#' || peek == ';') { continue; }
        }

        //error if something left unparsed
        if (!next("")) { uciOutput.error(command); }
    }
}

void Uci::ucinewgame() {
    SHOULD_BE_READY;

    startpos();
    searchControl.clear();
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true"))  { chessVariant = Chess960; return; }
        if (next("false")) { chessVariant = Orthodox; return; }

        io::fail_rewind(command);
        return;
    }

    SHOULD_BE_READY;

    if (next("Hash")) {
        next("value");

        HashMemory::size_t quantity = 0;

        if (command >> quantity) {
            io::char_type u = 'm';
            command >> u;

            switch (std::tolower(u)) {
                case 'g':
                    quantity *= 1024;

                case 'm':
                    quantity *= 1024;

                case 'k':
                    quantity *= 1024;

                case 'b':
                    break;

                default:
                    io::fail_rewind(command);
                    return;
            }

            searchControl.resizeHash(quantity);
            return;
        }

        io::fail_rewind(command);
        return;
    }

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

    searchMoves.generateMoves();

    auto& l = searchLimit;
    l.clear();
    l.perft = true; //DEBUG

    while (command) {
        if      (next("depth"))    { command >> l.depth; l.depth = std::min(l.depth, l.MaxDepth); }
        else if (next("wtime"))    { command >> l.time[white]; }
        else if (next("btime"))    { command >> l.time[black]; }
        else if (next("winc"))     { command >> l.inc[white]; }
        else if (next("binc"))     { command >> l.inc[black]; }
        else if (next("movestogo")){ command >> l.movestogo; }
        else if (next("nodes"))    { command >> l.nodes; }
        else if (next("movetime")) { command >> l.movetime; }
        else if (next("ponder"))   { l.ponder = true; }
        else if (next("infinite")) { l.infinite = true; }
        else if (next("perft"))    { l.perft = true; }
        else if (next("divide"))   { l.divide = true; }
        else if (next("searchmoves")) { searchMoves.limitMoves(command, colorToMove); }
        else if (next(""))         { break; }
        else {
            std::string token;
            command >> token;
            uciOutput.error(std::string("ignoring token: ") + token);
        }
    }

    searchControl.go(uciOutput, rootPosition, searchLimit);
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
