#include <fstream>

#include "Uci.hpp"
#include "PositionFen.hpp"
#include "PositionMoves.hpp"

Uci::Uci (std::ostream& out)
    : searchControl{}, uciOutput(out, chessVariant, colorToMove)
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
        else if (next("isready"))    { isready(); }
        else if (next("setoption"))  { setoption(); }
        else if (next("ucinewgame")) { ucinewgame(); }
        else if (next("uci"))  { uci(); }
        else if (next("quit")) { break; }
        //UCI extensions
        else if (next("exit")) { return exit(); }
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
        if (!next("")) {
            uciOutput.error(command);
        }
    }

    return in.bad();
}

void Uci::ucinewgame() {
    searchControl.clear();
    setStartpos();
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

    if (next("Hash")) {
        next("value");

        unsigned megabytes;
        if (command >> megabytes) {
            searchControl.tt().resizeMb(megabytes);
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

    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    if (next("startpos")) {
        setStartpos();
    }

    if (next("fen")) {
        PositionFen::read(command, rootPosition, colorToMove);
    }

    next("moves");

    PositionMoves{rootPosition}.makeMoves(command, colorToMove);
}

void Uci::setStartpos() {
    PositionFen::setStartpos(rootPosition, colorToMove);
}

void Uci::go() {
    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    goLimit.read(command, rootPosition, colorToMove);
    searchControl.go(uciOutput, rootPosition, goLimit);
}

void Uci::uci() const {
    uciOutput.uci(searchControl);
}

void Uci::isready() const {
    uciOutput.isready(searchControl);
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
        std::ostringstream message;
        message << "error opening file " << filename;
        uciOutput.error(message.str());
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
