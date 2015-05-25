#include <fstream>

#include "Uci.hpp"
#include "PositionFen.hpp"
#include "PositionMoves.hpp"

#define SHOULD_BE_READY  if (!searchControl.isReady()) { io::fail_rewind(command); return; }

Uci::Uci (std::ostream& out):
    searchControl{},
    uciHash(searchControl.tt()),
    uciOutput(out, uciHash, chessVariant, colorToMove),
    searchMoves(rootPosition),
    goLimit(searchMoves),
    chessVariant(Orthodox)
{
    ucinewgame();
}

int Uci::operator() (std::istream& in) {
    for (std::string commandLine; std::getline(in, commandLine); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(commandLine));
        command >> std::ws;

        if (next("position"))  { position(); }
        else if (next("go"))   { go(); }
        else if (next("stop")) { searchControl.stop(); }
        else if (next("isready"))    { uciOutput.isready(searchControl); }
        else if (next("setoption"))  { setoption(); }
        else if (next("ucinewgame")) { ucinewgame(); }
        else if (next("uci"))  { uciOutput.uciok(); }
        else if (next("quit")) { break; }
        //UCI extensions
        else if (next("exit")) { return exit(); }
        else if (next("wait")) { searchControl.wait(); }
        else if (next("echo")) { echo(); }
        else if (next("call")) { call(); }
        else {
            auto peek = command.peek();
            if (peek == '#' || peek == ';') {
                continue; //ignore comment line
            }
        }

        //error if something non-blank left
        if (!next("")) {
            uciOutput.error(command);
        }
    }

    return in.bad();
}

void Uci::ucinewgame() {
    SHOULD_BE_READY;

    startpos();
    searchControl.clear();
    uciHash.newGame();
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true")) {
            chessVariant = Chess960;
            return;
        }
        else if (next("false")) {
            chessVariant = Orthodox;
            return;
        }
    }

    SHOULD_BE_READY;

    if (next("Hash")) {
        next("value");

        UciHash::_t mebibytes;
        if (command >> mebibytes) {
            uciHash.resize(mebibytes);
            return;
        }
    }

    io::fail_rewind(command);
}

void Uci::position() {
    if (next("")) {
        uciOutput.info_fen(rootPosition);
        return;
    }

    SHOULD_BE_READY;

    if (next("startpos")) {
        startpos();
    }

    if (next("fen")) {
        PositionFen::read(command, rootPosition, colorToMove);
    }

    next("moves");

    PositionMoves::makeMoves(command, rootPosition, colorToMove);
}

void Uci::startpos() {
    PositionFen::setStartpos(rootPosition, colorToMove);
}

void Uci::go() {
    SHOULD_BE_READY;

    goLimit.read(command, colorToMove);

    if (next("")) {
        searchControl.go(uciOutput, goLimit);
    }
}

void Uci::echo() const {
    command >> std::ws;
    uciOutput.echo(command);
}

int Uci::exit() const {
    int exit_code = 0;
    command >> exit_code;
    return exit_code;
}

int Uci::call(const std::string& filename) {
    std::ifstream file(filename);

    if (!file) {
        uciOutput.error(std::string("error opening file: ") + filename);
        return 2;
    }

    return operator()(file);
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
