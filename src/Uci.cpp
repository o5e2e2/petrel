#include <fstream>

#include "Uci.hpp"
#include "SearchControl.hpp"
#include "PositionMoves.hpp"

Uci::Uci (std::ostream& out)
    : output(out, chessVariant, colorToMove), searchControl{}
{
    ucinewgame();
}

void Uci::ucinewgame() {
    searchControl.clear();
    set_startpos();
}

bool Uci::operator() (std::istream& in) {
    for (std::string command_line; std::getline(in, command_line); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(command_line));

        if (next("position"))  { position(); }
        else if (next("go"))   { go(); }
        else if (next("stop")) { searchControl.stop(); }
        else if (next("isready"))    { isready(); }
        else if (next("setoption"))  { setoption(); }
        else if (next("ucinewgame")) { ucinewgame(); }
        else if (next("uci"))  { output.uci(searchControl); }
        else if (next("quit")) { std::exit(EXIT_SUCCESS); }
        else if (next("wait")) { searchControl.wait(); }
        else if (next("echo")) { echo(); }
        else if (next("call")) { call(); }
        else if (next("exit")) { break; }
        else {
            auto peek = command.peek();
            if (command.eof() || peek == '#' || peek == ';') {
                continue; //ignore comment line
            }
        }

        //syntax error if something unparsed left
        if (!next("")) { output.error(command); }
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
        output.info_fen(startPosition);
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
    searchControl.go(output, startPosition, goLimit);
}

void Uci::isready() {
    output.isready( searchControl.isReady() );
}

void Uci::echo() {
    command >> std::ws;
    output.echo(command);
}

void Uci::call() {
    std::string filename;
    command >> filename;
    if (!operator()(filename)) { io::fail_rewind(command); }
}
