#include <fstream>
#include <cstdlib>

#include "Uci.hpp"
#include "SearchLimit.hpp"

#define SHOULD_BE_READY  if (!searchControl.isReady()) { io::fail_rewind(command); return; }

Uci::Uci (std::ostream& out, std::ostream& err): uciOutput(out, err), searchControl{}, rootMoves(rootPosition) {
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

        HashMemory::size_t quantity = 0;

        if (!(command >> quantity)) {
            io::fail_rewind(command);
            return;
        }

        io::char_type u = 'm';
        command >> u;

        switch (std::tolower(u)) {
            case 'g': quantity *= 1024;
            case 'm': quantity *= 1024;
            case 'k': quantity *= 1024;
            case 'b': break;

            default: {
                io::fail_rewind(command);
                return;
            }
        }

        searchControl.resizeHash(quantity);
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
    if (next("fen")) { uciOutput.setColorToMove( rootPosition.setFen(command) ); }

    next("moves");
    uciOutput.setColorToMove( rootMoves.makeMoves(command, uciOutput.getColorToMove()) );
}

void Uci::startpos() {
    uciOutput.setColorToMove( rootPosition.setStartpos() );
}

void Uci::go() {
    SHOULD_BE_READY;

    rootMoves.generateMoves();
    searchLimit.readUci(command, uciOutput.getColorToMove(), &rootMoves);

    //error if something left unparsed
    if (!next("")) { uciOutput.error(command); }

    searchControl.go(uciOutput, rootMoves, searchLimit);
}

void Uci::echo() {
    command >> std::ws;
    uciOutput.echo(command);
}

void Uci::quit() {
    int exit_code = 0;
    command >> exit_code;
    std::exit(exit_code);
}

void Uci::call(const std::string& filename) {
    std::ifstream file(filename);

    if (!file) {
        io::fail_rewind(command);
        return;
    }

    operator()(file);
}

void Uci::call() {
    std::string filename;
    command >> filename;

    if (filename.empty() || !next("")) {
        io::fail_rewind(command);
        return;
    }

    call(filename);
}

#undef SHOULD_BE_READY
