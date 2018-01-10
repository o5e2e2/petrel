#include "UciOutput.hpp"
#include "HashMemory.hpp"
#include "Move.hpp"
#include "OutputBuffer.hpp"
#include "UciPosition.hpp"

#define OUTPUT(ob) OutputBuffer<decltype(outLock)> ob(out, outLock)

namespace {
    template <typename T>
    T mebi(T bytes) { return bytes / (1024 * 1024); }

    template <typename T>
    constexpr T permil(T n, T m) { return (n * 1000) / m; }

    template <typename duration_type>
    auto milliseconds(duration_type duration)
        -> decltype(duration_cast<Milliseconds>(duration).count()) {
        return duration_cast<Milliseconds>(duration).count();
    }

    template <typename nodes_type, typename duration_type>
    nodes_type nps(nodes_type nodes, duration_type duration) {
        return (nodes * duration_type::period::den) / (duration.count() * duration_type::period::num);
    }
}

UciOutput::UciOutput (const UciPosition& p, io::ostream& o, io::ostream& e) :
    out(o),
    err(e),
    pos(p),
    isreadyWaiting{false},
    lastInfoNodes{0}
{}

void UciOutput::uciok(const HashMemory::Info& hashInfo) const {
    bool isChess960 = pos.getChessVariant().is(Chess960);

    OUTPUT(ob);
    ob << "id name " << io::app_version << '\n';
    ob << "id author Aleks Peshkov\n";
    ob << "option name UCI_Chess960 type check default " << (isChess960? "true" : "false") << '\n';
    ob << "option name Hash type spin min 0 max " << ::mebi(hashInfo.max) << " default " << ::mebi(hashInfo.current) << '\n';
    ob << "uciok\n";
}

void UciOutput::isready(bool searchIsReady) const {
    OUTPUT(ob);
    if (searchIsReady) {
        isreadyWaiting = false;
        ob << "readyok\n";
    }
    else {
        isreadyWaiting = true;
    }
}

void UciOutput::readyok() const {
    if (isreadyWaiting) {
        if (outLock.try_lock()) {
            isreadyWaiting = false;

            std::ostringstream ob;
            info_nps(ob);
            ob << "readyok\n";
            out << ob.str() << std::flush;
            outLock.unlock();
        }
    }
}

void UciOutput::bestmove() const {
    OUTPUT(ob);
    info_nps(ob);
    ob << "bestmove "; write(ob, _bestmove); ob << '\n';
    lastInfoNodes = 0;
}

void UciOutput::info_depth() const {
    OUTPUT(ob);
    ob << "info depth " << depth;
    nps(ob);
    ob << " score " << get(PerftNodes) << '\n';
}

void UciOutput::info_currmove() const {
    OUTPUT(ob);
    ob << "info currmovenumber " << currmovenumber;
    ob << " currmove "; write(ob, currmove);
    nps(ob);
    ob << " score " << get(PerftNodes) - get(PerftDivideNodes) << '\n';
}

void UciOutput::write(io::ostream& ob, const Move& move) const {
    Move::write(ob, move, pos.getColorToMove(), pos.getChessVariant());
}

void UciOutput::nps(io::ostream& ob) const {
    auto _nodes = getNodes();
    if (_nodes > 0 && lastInfoNodes != _nodes) {
        lastInfoNodes = _nodes;

        ob << " nodes " << _nodes;

        auto duration = fromSearchStart.getDuration();
        if (duration >= Milliseconds{1}) {
            ob << " time " << ::milliseconds(duration);

            if (duration >= Milliseconds{20}) {
                ob << " nps " << ::nps(_nodes, duration);
            }
        }

        if (get(TT_Tried) > 0) {
            ob << " hhits " << get(TT_Hit);
            ob << " hreads " << get(TT_Tried);
            ob << " hhitratio " << ::permil(get(TT_Hit), get(TT_Tried));
            ob << " hwrites " << get(TT_Written);
        }
    }
}

void UciOutput::info_nps(io::ostream& ob) const {
    std::ostringstream buffer;
    nps(buffer);

    if (!buffer.str().empty()) {
        ob << "info" << buffer.str() << '\n';
    }
}

void UciOutput::info_fen() const {
    OUTPUT(ob);
    ob << "info fen " << pos;
    //ob << " key 0x" << pos.getZobrist();
    ob << '\n';
}

void UciOutput::error(io::istream& in) const {
    OutputBuffer<decltype(outLock)>(err, outLock) << "parsing error: " << in.rdbuf() << '\n';
}

void UciOutput::error(const std::string& str) const {
    OutputBuffer<decltype(outLock)>(err, outLock) << str << '\n';
}

#undef OUTPUT
