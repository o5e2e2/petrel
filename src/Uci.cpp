#include "Uci.hpp"

#include <cstdlib>
#include <fstream>
#include <string>

#include "Clock.hpp"
#include "OutputBuffer.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "PositionFen.hpp"
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

Uci::Uci (SearchControl& s, std::ostream& out, std::ostream& err)
    : start_position(), output(s, out), uci_err(err)
{
    ucinewgame();
}

bool Uci::next(io::literal keyword) {
    return io::next(this->command, keyword);
}

bool Uci::operator() (const std::string& filename) {
    std::ifstream file(filename);
    return operator()(file);
}

void Uci::call() {
    std::string filename;
    command >> filename;
    if (!operator()(filename)) { io::fail_rewind(command); }
}

bool Uci::operator() (std::istream& in) {
    for (std::string command_line; std::getline(in, command_line); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(command_line));

        if (next("position"))  { position(); }
        else if (next("go"))   { go(); }
        else if (next("stop")) { output.search.stop(); }
        else if (next("isready"))    { output.isready(); }
        else if (next("setoption"))  { setoption(); }
        else if (next("ucinewgame")) { ucinewgame(); }
        else if (next("uci"))  { uci(); }
        else if (next("quit")) { std::exit(EXIT_SUCCESS); }
        else if (next("wait")) { output.search.wait(); }
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
        if (!next("")) { log_error(); }
    }

    return !in.bad();
}

void Uci::log_error() {
    OutputBuffer{uci_err} << "parsing error: " << command.rdbuf() << '\n';
}

void Uci::ucinewgame() {
    output.search.clear();
    set_startpos();
}

void Uci::uci() {
    auto max_mb = output.search.tt().getMaxSizeMb();
    auto current_mb = output.search.tt().getSizeMb();

    OutputBuffer{output.uci_out} << "id name " << io::app_version << '\n'
        << "id author Aleks Peshkov\n"
        << "option name UCI_Chess960 type check default " << (output.chessVariant == Chess960? "true":"false") << '\n'
        << "option name Hash type spin min 0 max " << max_mb << " default " << current_mb << '\n'
        << "uciok\n"
    ;
}

void Uci::setoption() {
    next("name");

    if (next("UCI_Chess960")) {
        next("value");

        if (next("true")) {
            output.chessVariant = Chess960;
        }
        else if (next("false")) {
            output.chessVariant = Orthodox;
        }

        return;
    }

    if (next("Hash")) {
        next("value");

        unsigned megabytes;
        if (command >> megabytes) {
            output.search.tt().resizeMb(megabytes);
        }

        return;
    }
}

void Uci::echo() {
    command >> std::ws;
    OutputBuffer{output.uci_out} << command.rdbuf() << '\n';
}

void Uci::position() {
    if (next("")) {
        OutputBuffer out{output.uci_out};
        out << "info fen ";
        PositionFen::write(out, start_position, output.colorToMove, output.chessVariant);
        out << '\n';
        return;
    }

    if (next("startpos")) {
        set_startpos();
    }

    if (next("fen")) {
        PositionFen::read(command, start_position, output.colorToMove);
    }

    next("moves");

    output.colorToMove = PositionMoves{start_position}.makeMoves(command, output.colorToMove);
}

void Uci::set_startpos() {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    PositionFen::read(startpos, start_position, output.colorToMove);
}

void Uci::go() {
    if (!output.search.isReady()) {
        io::fail_rewind(command);
        return;
    }

    read_go_limits(output.colorToMove);

    output.search.go(output, start_position, search_limit);
}

void Uci::read_go_limits(Color colorToMove) {
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
