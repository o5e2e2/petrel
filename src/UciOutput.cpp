#include "UciOutput.hpp"
#include "OutputBuffer.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "PositionFen.hpp"
#include "Move.hpp"

namespace {
    std::ostream& operator << (std::ostream& out, Clock::_t duration) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return out << milliseconds;
    }
}

UciOutput::UciOutput (std::ostream& o, const ChessVariant& v, const Color& c)
    : out(o), chessVariant(v), colorToMove(c), isreadyWaiting(false) {}

void UciOutput::uci(const SearchControl& search) const {
    auto max_mb = search.tt().getMaxSizeMb();
    auto current_mb = search.tt().getSizeMb();

    OutputBuffer{out}
        << "id name " << io::app_version << '\n'
        << "id author Aleks Peshkov\n"
        << "option name UCI_Chess960 type check default " << (chessVariant == Chess960? "true":"false") << '\n'
        << "option name Hash type spin min 0 max " << max_mb << " default " << current_mb << '\n'
        << "uciok\n"
    ;
}

void UciOutput::isready(const SearchControl& search) const {
    if (search.isReady()) {
        OutputBuffer{out} << "readyok\n";
        isreadyWaiting = false;
    }
    else {
        isreadyWaiting = true;
    }
}

void UciOutput::report_current(const SearchInfo& info) const {
    if (isreadyWaiting) {
        isreadyWaiting = false;

        OutputBuffer ob{out};
        info_nps(ob, info);
        ob << "readyok\n";
    }
}

void UciOutput::report_bestmove(const SearchInfo& info) const {
    OutputBuffer ob{out};
    info_nps(ob, info);
    ob << "bestmove ";
    write(ob, info.bestmove);
    ob << '\n';
}

void UciOutput::report_perft_divide(const SearchInfo& info) const {
    OutputBuffer ob{out};
    ob << "info currmovenumber " << info.currmovenumber << " currmove ";
    write(ob, info.currmove);
    nps(ob, info);
    ob << " string perft " << (info.perftNodes - info.perftDivide) << '\n';
}

void UciOutput::report_perft_depth(const SearchInfo& info) const {
    OutputBuffer ob{out};
    ob << "info depth " << info.depth;
    nps(ob, info);
    ob << " string perft " << info.perftNodes << '\n';
}

void UciOutput::write(std::ostream& ob, const Move& move) const {
    Move::write(ob, move, colorToMove, chessVariant);
}

void UciOutput::nps(std::ostream& ob, const SearchInfo& info) const {
    if (info.nodes > 0) {
        ob << " nodes " << info.nodes;

        Clock::_t duration = info.clock.read();

        if (duration > Clock::_t::zero()) {
            ob << " time " << duration;

            if (duration >= std::chrono::milliseconds{20}) {
                auto _nps = (info.nodes * Clock::_t::period::den) / (duration.count() * Clock::_t::period::num);
                ob << " nps " << _nps;
            }
        }
    }
}

void UciOutput::info_nps(std::ostream& ob, const SearchInfo& info) const {
    std::ostringstream buffer;
    nps(buffer, info);

    if (!buffer.str().empty()) {
        ob << "info" << buffer.str() << '\n';
    }
}

void UciOutput::info_fen(const Position& pos) const {
    OutputBuffer ob{out};
    ob << "info fen ";
    PositionFen::write(ob, pos, colorToMove, chessVariant);
    ob << '\n';
}

void UciOutput::echo(std::istream& in) const {
    OutputBuffer{out} << in.rdbuf() << '\n';
}

void UciOutput::error(std::istream& in) const {
    OutputBuffer{std::cerr} << "parsing error: " << in.rdbuf() << '\n';
}
