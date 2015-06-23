#include "UciOutput.hpp"
#include "OutputBuffer.hpp"
#include "Move.hpp"
#include "Position.hpp"
#include "PositionFen.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "UciHash.hpp"

namespace {
    std::ostream& operator << (std::ostream& out, Clock::_t duration) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return out << milliseconds;
    }

/*
    std::ostream& operator << (std::ostream& out, Bb bb) {
        FOR_INDEX(Rank, rank) {
            FOR_INDEX(File, file) {
                out << (bb[Square(file, rank)]? file : '.');
            }
            out << '\n';
        }
        return out;
    }

    std::ostream& operator << (std::ostream& out, PieceSet v) {
        auto flags(out.flags());

        out << std::hex;
        FOR_INDEX(Pi, pi) {
            if (v[pi]) {
                out << pi;
            }
            else {
                out << ".";
            }
        }

        out.flags(flags);
        return out;
    }

    std::ostream& operator << (std::ostream& out, VectorPiMask v) {
        return out << PieceSet{v};
    }
*/
}

UciOutput::UciOutput (std::ostream& o, const Color& c, HashMemory& h)
    : out(o), colorToMove(c), uciHash(h), chessVariant(Orthodox), isreadyWaiting(false) {}

void UciOutput::uciok() const {
    OutputBuffer ob{out};
    ob << "id name " << io::app_version << '\n';
    ob << "id author Aleks Peshkov\n";
    ob << "option name UCI_Chess960 type check default " << (chessVariant.is(Chess960)? "true" : "false") << '\n';
    uciHash.option(ob);
    ob << "uciok\n";
}

void UciOutput::setChess960(bool isChess960) {
    chessVariant = (isChess960? Chess960 : Orthodox);
}

void UciOutput::resizeHash(UciHash::_t mebibytes) {
    uciHash.resize(mebibytes);
}

void UciOutput::isready(const SearchControl& search) const {
    isreadyWaiting = true;

    if (search.isReady()) {
        isreadyWaiting = false;

        OutputBuffer{out} << "readyok\n";
    }
}

void UciOutput::readyok(const SearchInfo& info) const {
    if (isreadyWaiting) {
        isreadyWaiting = false;

        OutputBuffer ob{out};
        info_nps(ob, info);
        ob << "readyok\n";
    }
}

void UciOutput::bestmove(const SearchInfo& info) const {
    OutputBuffer ob{out};
    info_nps(ob, info);
    ob << "bestmove "; write(ob, info.bestmove); ob << '\n';
}

void UciOutput::info_depth(const SearchInfo& info) const {
    OutputBuffer ob{out};
    ob << "info depth " << info.depth;
    nps(ob, info);
    ob << " string perft " << info.perftNodes << '\n';
}

void UciOutput::info_currmove(const SearchInfo& info) const {
    OutputBuffer ob{out};
    ob << "info currmovenumber " << info.currmovenumber;
    ob << " currmove "; write(ob, info.currmove);

    nps(ob, info);
    ob << " string perft " << (info.perftNodes - info.perftDivide) << '\n';
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

        uciHash.hashfull(ob);
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
    //ob << " key 0x" << std::hex << pos.getZobrist();
    ob << '\n';
}

void UciOutput::echo(std::istream& in) const {
    OutputBuffer{out} << in.rdbuf() << '\n';
}

void UciOutput::error(std::istream& in) const {
    OutputBuffer{std::cerr} << "parsing error: " << in.rdbuf() << '\n';
}

void UciOutput::error(const std::string& str) const {
    OutputBuffer{std::cerr} << str << '\n';
}
