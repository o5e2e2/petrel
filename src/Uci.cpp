#include "Uci.hpp"
#include "SearchLimit.hpp"

#define SHOULD_BE_READY  if (!uciControl.isReady()) { io::fail_rewind(command); return; }

Uci::Uci (std::ostream& out, std::ostream& err):
    uciPosition(),
    uciOutput(uciPosition, out, err),
    uciControl(uciOutput)
{
    ucinewgame();
}

void Uci::operator() (std::istream& in) {
    for (std::string commandLine; std::getline(in, commandLine); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(commandLine));
        command >> std::ws;

        if      (next("go"))        { go(); }
        else if (next("position"))  { position(); }
        else if (next("stop"))      { uciControl.stop(); }
        else if (next("isready"))   { uciControl.readyok(); }
        else if (next("setoption")) { setoption(); }
        else if (next("set"))       { setoption(); }
        else if (next("ucinewgame")){ ucinewgame(); }
        else if (next("uci"))       { uciControl.uciok(); }
        else if (next("quit"))      { break; }

        //error if something left unparsed
        if (!next("")) { uciOutput.error(command); }
    }
}

void Uci::ucinewgame() {
    SHOULD_BE_READY;

    uciControl.clear();

    startpos();
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true"))  { uciPosition.setVariant(Chess960); return; }
        if (next("false")) { uciPosition.setVariant(Orthodox); return; }

        io::fail_rewind(command);
        return;
    }

    SHOULD_BE_READY;

    if (next("Hash")) {
        next("value");
        uciControl.readUciHash(command);
        return;
    }

}

void Uci::position() {
    if (next("")) {
        uciOutput.info_fen();
        return;
    }

    SHOULD_BE_READY;

    if (next("startpos")) { startpos(); }
    if (next("fen")) { uciPosition.setFen(command); }

    if (command) {
        unsigned fifty, moves;
        command >> fifty >> moves;
        command.clear(); //ignore missing optional 'fifty' and 'moves' fen fields
    }

    next("moves");
    uciPosition.makeMoves(command);
}

void Uci::startpos() {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    uciPosition.setFen(startpos);
}

void Uci::go() {
    SHOULD_BE_READY;

    SearchLimit searchLimit;
    searchLimit.readUci(command, &uciPosition);
    uciControl.go(uciPosition, searchLimit);
}

#undef SHOULD_BE_READY
