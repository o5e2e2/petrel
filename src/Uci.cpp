#include <fstream>
#include <cstdlib>

#include "Uci.hpp"
#include "SearchLimit.hpp"

#define SHOULD_BE_READY  if (!searchControl.isReady()) { io::fail_rewind(command); return; }

Uci::Uci (std::ostream& out, std::ostream& err): uciOutput(out, err), searchControl(uciOutput), rootMoves(0) {
    ucinewgame();
}

void Uci::operator() (std::istream& in) {
    for (std::string commandLine; std::getline(in, commandLine); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(commandLine));
        command >> std::ws;

        if      (next("go"))        { go(); }
        else if (next("position"))  { position(); }
        else if (next("stop"))      { searchControl.stop(); }
        else if (next("isready"))   { uciOutput.isready(searchControl); }
        else if (next("setoption")) { setoption(); }
        else if (next("set"))       { setoption(); }
        else if (next("ucinewgame")){ ucinewgame(); }
        else if (next("uci"))       { uciOutput.uciok(searchControl); }
        else if (next("quit"))      { break; }
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

    searchControl.clear();

    startpos();
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
        searchControl.uciSetHash(command);
        return;
    }

}

void Uci::position() {
    if (next("")) {
        uciOutput.info_fen( rootMoves.getPos() );
        return;
    }

    SHOULD_BE_READY;

    if (next("startpos")) { startpos(); }
    if (next("fen")) { uciOutput.setColorToMove( rootMoves.setFen(command) ); }

    next("moves");
    uciOutput.setColorToMove( rootMoves.makeMoves(command, uciOutput.getColorToMove()) );
}

void Uci::startpos() {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    uciOutput.setColorToMove( rootMoves.setFen(startpos) );
}

void Uci::go() {
    SHOULD_BE_READY;

    SearchLimit searchLimit;
    searchLimit.readUci(command, uciOutput.getColorToMove(), &rootMoves);
    searchControl.go(rootMoves, searchLimit);
}

#undef SHOULD_BE_READY
