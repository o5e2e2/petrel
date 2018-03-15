#include "Uci.hpp"

Uci::Uci (io::ostream& out, io::ostream& err):
    positionFen{},
    searchLimit{},
    info(positionFen, out, err),
    searchControl(info)
{
    ucinewgame();
}

void Uci::operator() (io::istream& in) {
    for (std::string commandLine; std::getline(in, commandLine); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(commandLine));
        command >> std::ws;

        if      (next("go"))        { go(); }
        else if (next("position"))  { position(); }
        else if (next("stop"))      { searchControl.stop(); }
        else if (next("isready"))   { info.isready( searchControl.isReady() ); }
        else if (next("setoption")) { setoption(); }
        else if (next("set"))       { setoption(); }
        else if (next("ucinewgame")){ ucinewgame(); }
        else if (next("uci"))       { info.uciok( searchControl.tt() ); }
        else if (next("quit"))      { break; }

        //error if something left unparsed
        if (!next("")) { info.error(command); }
    }
}

void Uci::ucinewgame() {
    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    searchControl.newGame();
    positionFen.setStartpos();
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true"))  { positionFen.setChessVariant(Chess960); return; }
        if (next("false")) { positionFen.setChessVariant(Orthodox); return; }

        io::fail_rewind(command);
        return;
    }

    if (next("Hash")) {
        next("value");
        setHash();
        return;
    }

}

void Uci::setHash() {
    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

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

    searchControl.tt().resize(quantity);
}

void Uci::position() {
    if (next("")) {
        info.info_fen();
        return;
    }

    positionFen.readUci(command);
}

void Uci::go() {
    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    searchLimit.readUci(command, positionFen);
    searchControl.go(searchLimit);
}
