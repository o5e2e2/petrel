#include "UciOutput.hpp"
#include "OutputBuffer.hpp"
#include "Move.hpp"
#include "Position.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"

namespace {
    std::ostream& operator << (std::ostream& out, Clock::_t duration) {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return out << milliseconds;
    }

    enum { MiB = 1024 * 1024 };
    constexpr HashMemory::size_t toMiB(HashMemory::size_t bytes) { return bytes / MiB; }

    template <typename T>
    constexpr T permil(T n, T m) { return (n * 1000 + m / 2) / m; }

}

UciOutput::UciOutput (std::ostream& o, const Color& c)
    : out(o), colorToMove(c), chessVariant{Orthodox}, isreadyWaiting{false} {}

void UciOutput::uciok(const SearchControl& search) const {
    auto& hashMemory = search.tt();
    auto current = hashMemory.getSize();
    auto max = hashMemory.getMax();

    Lock lock(outputLock);
    OutputBuffer ob{out};

    ob << "id name " << io::app_version << '\n';
    ob << "id author Aleks Peshkov\n";
    ob << "option name UCI_Chess960 type check default " << (chessVariant.is(Chess960)? "true" : "false") << '\n';
    ob << "option name Hash type spin min 0 max " << ::toMiB(max) << " default " << ::toMiB(current) << '\n';
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
    ob << " score " << info[PerftNodes] << '\n';
}

void UciOutput::info_currmove(const SearchInfo& info) const {
    Lock lock(outputLock);
    OutputBuffer ob{out};

    ob << "info currmovenumber " << info.currmovenumber;
    ob << " currmove "; write(ob, info.currmove);

    nps(ob, info);
    ob << " score " << info[PerftNodes] - info[PerftDivideNodes] << '\n';
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

        if (info[TT_Tried] > 0) {
            ob << " hashhit " << ::permil(info[TT_Hit], info[TT_Tried]);
            ob << " hits " << info[TT_Hit];
            ob << " writes " << info[TT_Written];
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
    pos.fen(ob, colorToMove, chessVariant);
    //ob << " key 0x" << pos.getZobrist();
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
