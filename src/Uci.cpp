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
#include "version.hpp"

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
            //en passant capture move encoded as pawn captures pawn
            assert (move.from().is<Rank5>());
            assert (move.to().is<Rank5>());
            out << move_from << Square{File{move_to}, (colorToMove == White)? Rank6:Rank3};
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

bool Uci::operator() (std::istream& in) {
    for (std::string command_line; std::getline(in, command_line); ) {
        command.clear(); //clear errors state from the previous command
        command.str(std::move(command_line));

        command >> std::ws;

        if (command == "position")  { position(); }
        else if (command == "go")   { go(); }
        else if (command == "stop") { search.stop(); }
        else if (command == "isready")    { isready(); }
        else if (command == "setoption")  { setoption(); }
        else if (command == "ucinewgame") { ucinewgame(); }
        else if (command == "uci")  { uci(); }
        else if (command == "quit") { std::exit(EXIT_SUCCESS); }
        else if (command == "wait") { search.wait(); }
        else if (command == "echo") { echo(); }
        else if (command == "call") { call(); }
        else if (command == "exit") { break; }
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
        ::fail_pos(command, 0);
        return;
    }

    SearchLimit searchLimit;
    searchLimit.read(command, start_position, colorToMove);
    if (!command && !command.eof()) {
        return;
    }

    search.go(*this, start_position, searchLimit);
}

void Uci::uci() {
    auto max_mb = search.ttMaxSize() / (1024*1024);
    auto current_mb = search.ttSize() / (1024*1024);

    OutputBuffer{uci_out} << "id name " << program_version << '\n'
        << "id author Aleks Peshkov\n"
        << "option name UCI_Chess960 type check default " << (chessVariant == Chess960? "true":"false") << '\n'
        << "option name Hash type spin min 0 max " << max_mb << " default " << current_mb << '\n'
        << "uciok\n"
    ;
}

void Uci::setoption() {
    if (command == "name") {
        if (command == "UCI_Chess960") {
            if (command == "value") {
                if (command == "true") {
                    chessVariant = Chess960;
                    return;
                }
                else if (command == "false") {
                    chessVariant = Orthodox;
                    return;
                }
            }
        }
        if (command == "Hash") {
            if (command == "value") {
                unsigned megabytes;
                if (command >> megabytes) {
                    search.ttResize(static_cast<std::size_t>(megabytes)*1024*1024);
                    return;
                }
            }
        }
    }
    ::fail_pos(command, 0);
}

void Uci::echo() {
    command >> std::ws;
    OutputBuffer{uci_out} << command.rdbuf() << '\n';
}

void Uci::call() {
    std::string filename;
    command >> filename;
    if (!operator()(filename)) { ::fail_pos(command, 0); }
}

void Uci::position() {
    if (command == "startpos") {
        set_startpos();
    }

    if (command == "fen") {
        PositionFen::read(command, start_position, colorToMove);
    }

    if (command == "moves") {
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
    ::write(out, move, colorToMove, chessVariant);
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

namespace {
    std::ostream& operator << (std::ostream& out, duration_t duration) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return out << milliseconds;
    }
}

void Uci::nps(std::ostream& out) const {
    SearchInfo info;
    search.getSearchInfo(info);
    node_count_t nodes = info.nodes;
    duration_t duration = info.duration;

    if (nodes > 0) {
        out << " nodes " << nodes;

        if (duration > duration_t::zero()) {
            out << " time " << duration;

            if (duration >= std::chrono::milliseconds{20}) {
                auto _nps = (nodes * duration_t::period::den) / (duration.count() * duration_t::period::num);
                out << " nps " << _nps;
            }
        }
    }
}

void Uci::info_nps(std::ostream& out) const {
    SearchInfo info;
    search.getSearchInfo(info);
    node_count_t nodes = info.nodes;

    if (nodes > 0) {
        out << "info";
        nps(out);
        out << '\n';
    }
}
