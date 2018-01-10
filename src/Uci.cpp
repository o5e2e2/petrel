#include "Uci.hpp"

#define SHOULD_BE_READY  if (!uciControl.isReady()) { io::fail_rewind(command); return; }

Uci::Uci (io::ostream& out, io::ostream& err): uciPosition(), uciControl(uciPosition, out, err) {
    ucinewgame();
}

void Uci::operator() (io::istream& in) {
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
        if (!next("")) { uciControl.error(command); }
    }
}

void Uci::ucinewgame() {
    SHOULD_BE_READY;
    uciControl.newGame();
    uciPosition.setStartpos();
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true"))  { uciPosition.setChessVariant(Chess960); return; }
        if (next("false")) { uciPosition.setChessVariant(Orthodox); return; }

        io::fail_rewind(command);
        return;
    }

    SHOULD_BE_READY;

    if (next("Hash")) {
        next("value");
        setHash();
        return;
    }

}

void Uci::setHash() {
    size_t quantity = 0;
    command >> quantity;
    if (!command) {
        io::fail_rewind(command);
        return;
    }

    io::char_type unit = 'm';
    command >> unit;

    switch (std::tolower(unit)) {
        case 't': quantity *= 1024;
        case 'g': quantity *= 1024;
        case 'm': quantity *= 1024;
        case 'k': quantity *= 1024;
        case 'b': break;

        default: {
            io::fail_rewind(command);
            return;
        }
    }

    uciControl.resizeHash(quantity);
}

void Uci::position() {
    if (next("")) {
        uciControl.info_fen();
        return;
    }

    uciPosition.readUci(command);
}

void Uci::go() {
    uciControl.go(command, uciPosition);
}

#undef SHOULD_BE_READY
