#include <fstream>

#include "Uci.hpp"
#include "Clock.hpp"
#include "OutputBuffer.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "PositionMoves.hpp"

namespace {
    std::istream& operator >> (std::istream& in, duration_t& duration) {
        long milliseconds;
        if (in >> milliseconds) {
            duration = std::chrono::duration_cast<duration_t>(std::chrono::milliseconds{milliseconds});
        }
        return in;
    }
}

Uci::Uci (SearchControl& s, std::ostream& out)
    : search(s), output(out, s, chessVariant, colorToMove)
{
    ucinewgame();
}

void Uci::ucinewgame() {
    search.clear();
    set_startpos();
}

bool Uci::operator() (std::istream& in) {
    for (std::string command_line; std::getline(in, command_line); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(command_line));

        if (next("position"))  { position(); }
        else if (next("go"))   { go(); }
        else if (next("stop")) { search.stop(); }
        else if (next("isready"))    { output.isready(); }
        else if (next("setoption"))  { setoption(); }
        else if (next("ucinewgame")) { ucinewgame(); }
        else if (next("uci"))  { output.uci(); }
        else if (next("quit")) { std::exit(EXIT_SUCCESS); }
        else if (next("wait")) { search.wait(); }
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

bool Uci::next(io::literal keyword) {
    return io::next(this->command, keyword);
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
            search.tt().resizeMb(megabytes);
        }

        return;
    }
}

void Uci::position() {
    if (next("")) {
        output.info_fen(start_position);
        return;
    }

    if (!search.isReady()) {
        io::fail_rewind(command);
        return;
    }

    if (next("startpos")) {
        set_startpos();
    }

    if (next("fen")) {
        PositionFen::read(command, start_position, colorToMove);
    }

    next("moves");

    colorToMove = PositionMoves{start_position}.makeMoves(command, colorToMove);
}

void Uci::set_startpos() {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    PositionFen::read(startpos, start_position, colorToMove);
}

void Uci::go() {
    if (!search.isReady()) {
        io::fail_rewind(command);
        return;
    }

    read_go_limits();

    search.go(output, start_position, search_limit);
}

void Uci::read_go_limits() {
    SearchLimit& s = search_limit;

    s = {};

    PositionMoves p(start_position);
    s.searchmoves = p.getMoves();

    while (command) {
        if      (next("depth"))    { command >> s.depth; }
        else if (next("wtime"))    { command >> ((colorToMove == White)? s.time:s.op_time); }
        else if (next("btime"))    { command >> ((colorToMove == Black)? s.time:s.op_time); }
        else if (next("winc"))     { command >> ((colorToMove == White)? s.inc:s.op_inc); }
        else if (next("binc"))     { command >> ((colorToMove == Black)? s.inc:s.op_inc); }
        else if (next("movestogo")){ command >> s.movestogo; }
        else if (next("nodes"))    { command >> s.nodes; }
        else if (next("movetime")) { command >> s.movetime; }
        else if (next("ponder"))   { s.ponder = true; }
        else if (next("infinite")) { s.infinite = true; }
        else if (next("searchmoves")) { p.limitMoves(command, s.searchmoves, colorToMove); }
        else if (next("perft"))    { s.perft = true; }
        else if (next("divide"))   { s.divide = true; }
        else { break; }
    }
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
