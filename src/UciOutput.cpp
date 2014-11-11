#include <iostream>

#include "UciOutput.hpp"
#include "OutputBuffer.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "PositionFen.hpp"

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

    std::ostream& operator << (std::ostream& out, duration_t duration) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return out << milliseconds;
    }

    enum nps_write_t { Raw, InfoPrefix };
    template <nps_write_t Info = Raw>
    std::ostream& nps(std::ostream& out, const SearchInfo& info) {
        if (info.nodes > 0) {
            if (Info == InfoPrefix) {
                out << "info";
            }

            out << " nodes " << info.nodes;

            duration_t duration = info.clock.read();

            if (duration > duration_t::zero()) {
                out << " time " << duration;

                if (duration >= std::chrono::milliseconds{20}) {
                    auto _nps = (info.nodes * duration_t::period::den) / (duration.count() * duration_t::period::num);
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

UciOutput::UciOutput (std::ostream& out, const ChessVariant& v, const Color& c)
    : uci_out(out), isready_waiting(false), chessVariant(v), colorToMove(c) {}

void UciOutput::uci(const SearchControl& search) {
    auto max_mb = search.tt().getMaxSizeMb();
    auto current_mb = search.tt().getSizeMb();

    OutputBuffer{uci_out} << "id name " << io::app_version << '\n'
        << "id author Aleks Peshkov\n"
        << "option name UCI_Chess960 type check default " << (chessVariant == Chess960? "true":"false") << '\n'
        << "option name Hash type spin min 0 max " << max_mb << " default " << current_mb << '\n'
        << "uciok\n"
    ;
}

void UciOutput::isready(bool ready) {
    if (ready) {
        OutputBuffer{uci_out} << "readyok\n";
    }
    else {
        isready_waiting = true;
    }
}

void UciOutput::write_info_current(const SearchInfo& info) {
    if (isready_waiting) {
        isready_waiting = false;

        OutputBuffer out{uci_out};
        info_nps(out, info);
        out << "readyok\n";
    }
}

void UciOutput::write(std::ostream& out, Move move) const {
    ::write(out, move, colorToMove, chessVariant);
}

void UciOutput::report_bestmove(const SearchInfo& info) {
    OutputBuffer out{uci_out};
    info_nps(out, info);
    out << "bestmove ";
    write(out, info.bestmove);
    out << '\n';
}

void UciOutput::report_perft(const SearchInfo& info) {
    OutputBuffer out{uci_out};
    out << "info currmovenumber " << info.currmovenumber << " currmove ";
    write(out, info.currmove);
    nps(out, info);
    out << " string perft " << info.perft << '\n';
}

void UciOutput::report_perft_depth(const SearchInfo& info) {
    OutputBuffer out{uci_out};
    out << "info depth " << info.depth;
    nps(out, info);
    out << " string perft " << info.perft << '\n';
}

void UciOutput::nps(std::ostream& out, const SearchInfo& info) const {
    ::nps<>(out, info);
}

void UciOutput::info_nps(std::ostream& out, const SearchInfo& info) const {
    ::nps<InfoPrefix>(out, info);
}

void UciOutput::info_fen(const Position& pos) {
    OutputBuffer out{uci_out};
    out << "info fen ";
    PositionFen::write(out, pos, colorToMove, chessVariant);
    out << '\n';
}

void UciOutput::echo(std::istream& in) {
    OutputBuffer{uci_out} << in.rdbuf() << '\n';
}

void UciOutput::error(std::istream& in) {
    OutputBuffer{std::cerr} << "parsing error: " << in.rdbuf() << '\n';
}
