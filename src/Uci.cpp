#include "Uci.hpp"

Uci::Uci (io::ostream& out):
    positionFen{},
    info(out, positionFen.getColorToMove(), positionFen.getChessVariant()),
    searchControl(info)
{
    ucinewgame();
}

void Uci::operator() (io::istream& in, io::ostream& err) {
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

        //report error if something left unparsed
        if (!next("")) {
            err << "parsing error: " << command.rdbuf() << '\n';
        }
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
        searchControl.setHash(command);
        return;
    }

}

void Uci::position() {
    if (next("")) {
        std::stringstream ob;
        ob << "info fen " << positionFen << '\n';
        info(ob);
        return;
    }

    positionFen.readUci(command);
}

void Uci::go() {
    if (!searchControl.isReady()) {
        io::fail_rewind(command);
        return;
    }

    searchControl.go(command, positionFen);
}
