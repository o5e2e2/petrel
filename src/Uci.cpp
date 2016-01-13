#include <fstream>
#include <cstdlib>

#include "Uci.hpp"
#include "PositionMoves.hpp"
#include "SearchLimit.hpp"

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

Uci::Uci (std::ostream& out): searchControl(), uciOutput(out, colorToMove) {
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
        else if (next("uci"))       { uciOutput.uciok(searchControl); }
        else if (next("quit"))      { quit(); }
        //UCI extensions
        else if (next("exit"))      { break; }
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

        if (next("true"))  { uciOutput.set(Chess960); return; }
        if (next("false")) { uciOutput.set(Orthodox); return; }

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
    if (next("fen")) { rootPosition.setFen(command, colorToMove); }

    next("moves");
    rootPosition.makeMoves(command, &colorToMove);
}

void Uci::startpos() {
    rootPosition.setStartpos(colorToMove);
}

void Uci::go() {
    SHOULD_BE_READY;

    Side white(colorToMove.is(White)? My : Op);
    Side black(colorToMove.is(Black)? My : Op);

    PositionMoves searchMoves(rootPosition);
    searchMoves.generateMoves();

    SearchLimit l;
    {
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
    }

    searchControl.go(uciOutput, rootPosition, std::move(l));
}

void Uci::echo() const {
    command >> std::ws;
    uciOutput.echo(command);
}

void Uci::quit() const {
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
