#include "UciOutput.hpp"
#include "OutputBuffer.hpp"
#include "Move.hpp"
#include "Position.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"

#define OUTPUT(ob) OutputBuffer<decltype(outLock)> ob(out, outLock)

namespace {
    template <typename T>
    T mebi(T bytes) { return bytes / (1024 * 1024); }

    template <typename T>
    constexpr T permil(T n, T m) { return (n * 1000) / m; }

    template <typename Duration>
    auto milliseconds(Duration duration)
        -> decltype(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    template <typename Nodes, typename Duration>
    Nodes nps(Nodes nodes, Duration duration) {
        return (nodes * Duration::period::den) / (duration.count() * Duration::period::num);
    }
}

UciOutput::UciOutput (std::ostream& o, std::ostream& e) :
    out(o),
    err(e),
    colorToMove{White},
    chessVariant{Orthodox},
    isreadyWaiting{false},
    lastInfoNodes{0}
{}

void UciOutput::uciok(const SearchControl& search) const {
    auto& hashMemory = search.tt();
    auto current = hashMemory.getSize();
    auto max = hashMemory.getMax();

    OUTPUT(ob);
    ob << "id name " << io::app_version << '\n';
    ob << "id author Aleks Peshkov\n";
    ob << "option name UCI_Chess960 type check default " << (chessVariant.is(Chess960)? "true" : "false") << '\n';
    ob << "option name Hash type spin min 0 max " << ::mebi(max) << " default " << ::mebi(current) << '\n';
    ob << "uciok\n";
}

void UciOutput::isready(const SearchControl& search) const {
    if (search.isReady()) {
        isreadyWaiting = false;

        OutputBuffer<decltype(outLock)>(out, outLock) << "readyok\n";
    }
    else {
        isreadyWaiting = true;
    }
}

void UciOutput::readyok(const SearchInfo& info) const {
    if (isreadyWaiting) {
        if (outLock.try_lock()) {
            isreadyWaiting = false;

            std::ostringstream ob;
            info_nps(ob, info);
            ob << "readyok\n";
            out << ob.str() << std::flush;
            outLock.unlock();
        }
    }
}

void UciOutput::bestmove(const SearchInfo& info) const {
    OUTPUT(ob);
    info_nps(ob, info);
    ob << "bestmove "; write(ob, info.bestmove); ob << '\n';
    lastInfoNodes = 0;
}

void UciOutput::info_depth(const SearchInfo& info) const {
    OUTPUT(ob);
    ob << "info depth " << info.depth;
    nps(ob, info);
    ob << " score " << info[PerftNodes] << '\n';
}

void UciOutput::info_currmove(const SearchInfo& info) const {
    OUTPUT(ob);
    ob << "info currmovenumber " << info.currmovenumber;
    ob << " currmove "; write(ob, info.currmove);
    nps(ob, info);
    ob << " score " << info[PerftNodes] - info[PerftDivideNodes] << '\n';
}

void UciOutput::write(std::ostream& ob, const Move& move) const {
    Move::write(ob, move, colorToMove, chessVariant);
}

void UciOutput::nps(std::ostream& ob, const SearchInfo& info) const {
    auto nodes = info.getNodes();
    if (nodes > 0 && lastInfoNodes != nodes) {
        lastInfoNodes = nodes;

        ob << " nodes " << nodes;

        auto duration = info.clock.read();
        if (duration >= std::chrono::milliseconds{1}) {
            ob << " time " << ::milliseconds(duration);

            if (duration >= std::chrono::milliseconds{20}) {
                ob << " nps " << ::nps(nodes, duration);
            }
        }

        if (info[TT_Tried] > 0) {
            ob << " hhits " << info[TT_Hit];
            ob << " hreads " << info[TT_Tried];
            ob << " hhitratio " << ::permil(info[TT_Hit], info[TT_Tried]);
            ob << " hwrites " << info[TT_Written];
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
    OUTPUT(ob);
    ob << "info fen ";
    pos.fen(ob, colorToMove, chessVariant);
    //ob << " key 0x" << pos.getZobrist();
    ob << '\n';
}

void UciOutput::error(std::istream& in) const {
    OutputBuffer<decltype(outLock)>(err, outLock) << "parsing error: " << in.rdbuf() << '\n';
}

void UciOutput::error(const std::string& str) const {
    OutputBuffer<decltype(outLock)>(err, outLock) << str << '\n';
}

#undef OUTPUT
