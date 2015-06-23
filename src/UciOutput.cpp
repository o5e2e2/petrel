#include "UciOutput.hpp"
#include "OutputBuffer.hpp"
#include "Move.hpp"
#include "Position.hpp"
#include "PositionFen.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "PerftTT.hpp"

namespace {
    std::ostream& operator << (std::ostream& out, Clock::_t duration) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return out << milliseconds;
    }

    enum { MiB = 1024 * 1024 };
    constexpr HashMemory::size_t toMiB(HashMemory::size_t bytes) { return bytes / MiB; }

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

UciOutput::UciOutput (std::ostream& o, const Color& c, const ChessVariant& v, const HashMemory& h)
    : out(o), colorToMove(c), chessVariant(v), hashMemory(h), isreadyWaiting(false) {}

void UciOutput::uciok() const {
    auto current = hashMemory.getSize();
    auto max = hashMemory.getMax();

    Lock lock(outputLock);
    OutputBuffer ob{out};

    ob << "id name " << io::app_version << '\n';
    ob << "id author Aleks Peshkov\n";
    ob << "option name UCI_Chess960 type check default " << (chessVariant.is(Chess960)? "true" : "false") << '\n';
    ob << "option name Hash type spin min 0 max " << toMiB(max) << " default " << toMiB(current) << '\n';
    ob << "uciok\n";
}

void UciOutput::isready(const SearchControl& search) const {
    Lock lock(outputLock);

    if (search.isReady()) {
        isreadyWaiting = false;

        OutputBuffer{out} << "readyok\n";
    }
    else {
        isreadyWaiting = true;
    }
}

void UciOutput::readyok(const SearchInfo& info) const {
    if (outputLock.try_lock()) {
        if (isreadyWaiting) {
            isreadyWaiting = false;

            OutputBuffer ob{out};
            info_nps(ob, info);
            ob << "readyok\n";
        }

        outputLock.unlock();
    }
}

void UciOutput::bestmove(const SearchInfo& info) const {
    Lock lock(outputLock);
    OutputBuffer ob{out};

    info_nps(ob, info);
    ob << "bestmove "; write(ob, info.bestmove); ob << '\n';
}

void UciOutput::info_depth(const SearchInfo& info) const {
    Lock lock(outputLock);
    OutputBuffer ob{out};

    ob << "info depth " << info.depth;
    nps(ob, info);
    ob << " string perft " << info.perftNodes << '\n';
}

void UciOutput::info_currmove(const SearchInfo& info) const {
    Lock lock(outputLock);
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

        auto used  = PerftTT::getUsed();
        if (used > 0) {
            auto total = hashMemory.getTotalRecords();
            auto hf = (static_cast<HashMemory::size_t>(used)*1000) / total;
            hf = std::min(hf, static_cast<decltype(hf)>(1000));
            ob << " hashfull " << hf;
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
    Lock lock(outputLock);
    OutputBuffer ob{out};

    ob << "info fen ";
    PositionFen::write(ob, pos, colorToMove, chessVariant);
    //ob << " key 0x" << std::hex << pos.getZobrist();
    ob << '\n';
}

void UciOutput::echo(std::istream& in) const {
    Lock lock(outputLock);
    OutputBuffer{out} << in.rdbuf() << '\n';
}

void UciOutput::error(std::istream& in) const {
    OutputBuffer{std::cerr} << "parsing error: " << in.rdbuf() << '\n';
}

void UciOutput::error(const std::string& str) const {
    OutputBuffer{std::cerr} << str << '\n';
}
