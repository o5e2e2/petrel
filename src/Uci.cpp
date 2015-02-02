#include <fstream>

#include "Uci.hpp"
#include "SearchControl.hpp"
#include "PositionMoves.hpp"

Uci::Uci (std::ostream& out)
    : searchControl{}, uciOutput(out, chessVariant, colorToMove)
{
    ucinewgame();
}

void Uci::ucinewgame() {
    searchControl.clear();
    set_startpos();
}

bool Uci::operator() (std::istream& in) {
    for (std::string commandLine; std::getline(in, commandLine); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(commandLine));
        command >> std::ws;

        if (next("position"))  { position(); }
        else if (next("go"))   { go(); }
        else if (next("stop")) { searchControl.stop(); }
        else if (next("isready"))    { isready(); }
        else if (next("setoption"))  { setoption(); }
        else if (next("ucinewgame")) { ucinewgame(); }
        else if (next("uci"))  { uciOutput.uci(searchControl); }
        else if (next("quit")) { std::exit(EXIT_SUCCESS); }
        //UCI extensions
        else if (next("exit")) { break; }
        else if (next("wait")) { searchControl.wait(); }
        else if (next("echo")) { echo(); }
        else if (next("call")) { call(); }
        else {
            auto peek = command.peek();
            if (command.eof() || peek == '#' || peek == ';') {
                continue; //ignore empty or comment line
            }
        }

        //syntax error if something unparsed left
        if (!next("")) { uciOutput.error(command); }
    }

    return !in.bad();
}

bool Uci::operator() (const std::string& filename) {
    std::ifstream file(filename);
    return operator()(file);
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true")) {
            chessVariant = Chess960;
        }
        else if (next("false")) {
            chessVariant = Orthodox;
        }

        return;
    }

    if (next("Hash")) {
        next("value");

        unsigned megabytes;
        if (command >> megabytes) {
            searchControl.tt().resizeMb(megabytes);
        }

        return;
    }
}

void Uci::position() {
    if (next("")) {
        uciOutput.info_fen(startPosition);
        return;
    }

    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    if (next("startpos")) {
        set_startpos();
    }

    if (next("fen")) {
        PositionFen::read(command, startPosition, colorToMove);
    }

    next("moves");

    colorToMove = PositionMoves{startPosition}.makeMoves(command, colorToMove);
}

void Uci::set_startpos() {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    PositionFen::read(startpos, startPosition, colorToMove);
}

void Uci::go() {
    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    goLimit.read(command, startPosition, colorToMove);
    searchControl.go(uciOutput, startPosition, goLimit);
}

void Uci::isready() {
    uciOutput.isready( searchControl.isReady() );
}

void Uci::echo() {
    command >> std::ws;
    uciOutput.echo(command);
}

void Uci::call() {
    std::string filename;
    command >> filename;
    if (!operator()(filename)) { io::fail_rewind(command); }
}
