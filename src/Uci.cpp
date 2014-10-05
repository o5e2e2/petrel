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
    //convert internal move to long algebraic format
    std::ostream& write(std::ostream& out, Move move, Color colorToMove, ChessVariant chessVariant) {
        if (move.isNull()) { return out << "0000"; }

        Square move_from = (colorToMove == White)? move.from():~move.from();
        Square move_to = (colorToMove == White)? move.to():~move.to();

        if (!move.isSpecial()) {
            out << move_from << move_to;
        }
        else {
            if (move.from().is<Rank7>()) {
                //the type of a promoted pawn piece encoded in place of to's rank
                Square promoted_to(File(move_to), (colorToMove == White)? Rank8:Rank1);
                PromoType promo = Move::decodePromoType(move.to());
                out << move_from << promoted_to << promo;
            }
            else if (move.from().is<Rank1>()) {
                //castling move internally encoded as the rook captures the king
                if (chessVariant == Chess960) {
                    out << move_to << move_from;
                }
                else {
                    if (move_from.is<FileA>()) {
                        out << move_to << Square{FileC, Rank{move_from}};
                    }
                    else {
                        assert (move_from.is<FileH>());
                        out << move_to << Square{FileG, Rank{move_from}};
                    }
                }
            }
            else {
                //en passant capture move internally encoded as pawn captures pawn
                assert (move.from().is<Rank5>());
                assert (move.to().is<Rank5>());
                out << move_from << Square{File{move_to}, (colorToMove == White)? Rank6:Rank3};
            }
        }
        return out;
    }

    std::istream& operator >> (std::istream& in, duration_t& duration) {
        long milliseconds;
        if (in >> milliseconds) {
            duration = std::chrono::duration_cast<duration_t>(std::chrono::milliseconds{milliseconds});
        }
        return in;
    }

    std::ostream& operator << (std::ostream& out, duration_t duration) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return out << milliseconds;
    }

    enum nps_write_t { Raw, InfoPrefix };
    template <nps_write_t Info = Raw>
    std::ostream& nps(std::ostream& out, node_count_t nodes, duration_t duration) {
        if (nodes > 0) {
            if (Info == InfoPrefix) {
                out << "info";
            }

            out << " nodes " << nodes;

            if (duration > duration_t::zero()) {
                out << " time " << duration;

                if (duration >= std::chrono::milliseconds{20}) {
                    auto _nps = (nodes * duration_t::period::den) / (duration.count() * duration_t::period::num);
                    out << " nps " << _nps;
                }
            }

            if (Info == InfoPrefix) {
                out << '\n';
            }
        }
        return out;
    }

}

Uci::Uci (SearchControl& s, std::ostream& out, std::ostream& err)
    : start_position{}, search(s), uci_out(out), uci_err(err), isready_waiting{false}
{
    ucinewgame();
}

void Uci::ucinewgame() {
    search.clear();
    set_startpos();
}

bool Uci::next(io::literal keyword) {
    return io::next(this->command, keyword);
}

bool Uci::operator() (std::istream& in) {
    for (std::string command_line; std::getline(in, command_line); ) {
        command.clear(); //clear errors from the previous command
        command.str(std::move(command_line));

        if (next("position"))  { position(); }
        else if (next("go"))   { go(); }
        else if (next("stop")) { search.stop(); }
        else if (next("isready"))    { isready(); }
        else if (next("setoption"))  { setoption(); }
        else if (next("ucinewgame")) { ucinewgame(); }
        else if (next("uci"))  { uci(); }
        else if (next("quit")) { std::exit(EXIT_SUCCESS); }
        else if (next("wait")) { search.wait(); }
        else if (next("echo")) { echo(); }
        else if (next("call")) { call(); }
        else if (next("exit")) { break; }
        else {
            auto peek = command.peek();
            if (command.eof() || peek == '#' || peek == ';') {
                continue; //ignore comment
            }
        }

        command >> std::ws;
        if (!command.eof()) { log_error(); }
    }

    return !in.bad();
}

void Uci::log_error() {
    OutputBuffer{uci_err} << "parsing error: " << command.rdbuf() << '\n';
}

bool Uci::operator() (const std::string& filename) {
    std::ifstream file{filename};
    return operator()(file);
}

void Uci::go() {
    if (!search.isReady()) {
        io::fail_rewind(command);
        return;
    }

    read_go_limits();

    if (!command && !command.eof()) {
        return;
    }

    search.go(*this, start_position, search_limit);
}

void Uci::uci() {
    auto max_mb = search.ttMaxSize() / (1024*1024);
    auto current_mb = search.ttSize() / (1024*1024);

    OutputBuffer{uci_out} << "id name " << io::app_version << '\n'
        << "id author Aleks Peshkov\n"
        << "option name UCI_Chess960 type check default " << (chessVariant == Chess960? "true":"false") << '\n'
        << "option name Hash type spin min 0 max " << max_mb << " default " << current_mb << '\n'
        << "uciok\n"
    ;
}

void Uci::setoption() {
    if (next("name")) {
        if (next("UCI_Chess960")) {
            if (next("value")) {
                if (next("true")) {
                    chessVariant = Chess960;
                    return;
                }
                else if (next("false")) {
                    chessVariant = Orthodox;
                    return;
                }
            }
        }
        if (next("Hash")) {
            if (next("value")) {
                unsigned megabytes;
                if (command >> megabytes) {
                    search.ttResize(static_cast<std::size_t>(megabytes)*1024*1024);
                    return;
                }
            }
        }
    }
    io::fail_rewind(command);
}

void Uci::echo() {
    command >> std::ws;
    OutputBuffer{uci_out} << command.rdbuf() << '\n';
}

void Uci::call() {
    std::string filename;
    command >> filename;
    if (!operator()(filename)) { io::fail_rewind(command); }
}

void Uci::position() {
    if (next("startpos")) {
        set_startpos();
    }

    if (next("fen")) {
        PositionFen::read(command, start_position, colorToMove);
    }

    if (next("moves")) {
        colorToMove = PositionMoves{start_position}.makeMoves(command, colorToMove);
    }
}

void Uci::set_startpos() {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    PositionFen::read(startpos, start_position, colorToMove);
}

void Uci::isready() {
    if (search.isReady()) {
        OutputBuffer{uci_out} << "readyok\n";
    }
    else {
        isready_waiting = true;
    }
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

void Uci::write_info_current() {
    if (isready_waiting) {
        isready_waiting = false;

        OutputBuffer out{uci_out};
        out << "info fen ";
        PositionFen::write(out, start_position, colorToMove, chessVariant);
        out << '\n';
        info_nps(out);
        out << "readyok\n";
    }
}

void Uci::write(std::ostream& out, Move move) const {
    ::write(out, move, this->colorToMove, this->chessVariant);
}

void Uci::report_bestmove(Move move) {
    OutputBuffer out{uci_out};
    info_nps(out);
    out << "bestmove ";
    write(out, move);
    out << '\n';
}

void Uci::report_perft_depth(depth_t depth, node_count_t perftNodes) {
    OutputBuffer out{uci_out};
    out << "info depth " << depth;
    nps(out);
    out << " string perft " << perftNodes << '\n';
}

void Uci::report_perft(Move move, index_t currmovenumber, node_count_t perftNodes) {
    OutputBuffer out{uci_out};
    out << "info currmovenumber " << currmovenumber << " currmove ";
    write(out, move);
    nps(out);
    out << " string perft " << perftNodes << '\n';
}

void Uci::nps(std::ostream& out) const {
    SearchInfo info;
    search.getSearchInfo(info);
    node_count_t nodes = info.nodes;
    duration_t duration = info.duration;

    ::nps<>(out, nodes, duration);

}

void Uci::info_nps(std::ostream& out) const {
    SearchInfo info;
    search.getSearchInfo(info);
    node_count_t nodes = info.nodes;
    duration_t duration = info.duration;

    ::nps<InfoPrefix>(out, nodes, duration);
}
