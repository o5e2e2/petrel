#include "UciSearchInfo.hpp"
#include "Milliseconds.hpp"
#include "Move.hpp"
#include "OutputBuffer.hpp"
#include "PerftTT.hpp"
#include "PositionFen.hpp"

#define OUTPUT(ob) OutputBuffer<decltype(outLock)> ob(out, outLock)

namespace {
    template <typename T>
    T mebi(T bytes) { return bytes / (1024 * 1024); }

    template <typename T>
    constexpr T permil(T n, T m) { return (n * 1000) / m; }

    io::ostream& operator << (io::ostream& out, Score a) {
        return out << static_cast<score_t>(a);
    }
}

UciSearchInfo::UciSearchInfo (io::ostream& o, const PositionFen& p) :
    out(o),
    positionFen(p)
{}

void UciSearchInfo::position() const {
    OUTPUT(ob);
    ob << "info fen " << positionFen << '\n';
}

void UciSearchInfo::clear() {
    lastInfoNodes = 0;
    fromSearchStart = {};
}

void UciSearchInfo::uciok(const HashInfo& hashInfo) const {
    bool isChess960 = positionFen.isChess960();

    OUTPUT(ob);
    ob << "id name petrel\n";
    ob << "id author Aleks Peshkov\n";
    ob << "option name UCI_Chess960 type check default " << (isChess960 ? "true" : "false") << '\n';
    ob << "option name Hash type spin min 0 max " << ::mebi(hashInfo.maxSize) << " default " << ::mebi(hashInfo.currentSize) << '\n';
    ob << "uciok\n";
}

void UciSearchInfo::isready(bool searchIsReady) const {
    OUTPUT(ob);
    if (searchIsReady) {
        isreadyWaiting = false;
        ob << "readyok\n";
    }
    else {
        isreadyWaiting = true;
    }
}

void UciSearchInfo::readyok(node_count_t nodes, const PerftTT& tt) const {
    if (isreadyWaiting) {
        std::ostringstream ob;
        info_nps(ob, nodes, tt);
        ob << "readyok\n";

        if (outLock.try_lock()) {
            if (isreadyWaiting) {
                isreadyWaiting = false;
                out << ob.str() << std::flush;
            }
            outLock.unlock();
        }
    }
}

void UciSearchInfo::write(io::ostream& o, const Move& move) const {
    positionFen.writeMove(o, move);
}

void UciSearchInfo::nps(io::ostream& o, node_count_t nodes, const PerftTT& tt) const {
    if (lastInfoNodes == nodes) {
        return;
    }
    lastInfoNodes = nodes;

    o << " nodes " << nodes;

    auto duration = fromSearchStart.getDuration();
    if (duration >= Milliseconds{1}) {
        o << " time " << ::milliseconds(duration);

        if (duration >= Milliseconds{20}) {
            o << " nps " << ::nps(nodes, duration);
        }
    }

    auto& counter = tt.getCounter();
    if (counter.reads > 0) {
        o << " hhits " << counter.hits;
        o << " hreads " << counter.reads;
        o << " hhitratio " << ::permil(counter.hits, counter.reads);
        o << " hwrites " << counter.writes;
    }
}

void UciSearchInfo::info_nps(io::ostream& o, node_count_t nodes, const PerftTT& tt) const {
    std::ostringstream buffer;
    nps(buffer, nodes, tt);

    if (!buffer.str().empty()) {
        o << "info" << buffer.str() << '\n';
    }
}

void UciSearchInfo::bestmove(const Move& bestMove, Score bestScore, node_count_t nodes, const PerftTT& tt) const {
    OUTPUT(ob);
    if (lastInfoNodes != nodes) {
        ob << "info";
        nps(ob, nodes, tt);
        ob << " score " << bestScore << '\n';
    }
    ob << "bestmove "; write(ob, bestMove); ob << '\n';
}

void UciSearchInfo::report_perft_depth(depth_t draft, const Move& bestMove, Score bestScore, node_count_t perftNodes, node_count_t nodes, const PerftTT& tt) const {
    OUTPUT(ob);
    ob << "info depth " << draft;
    nps(ob, nodes, tt);
    ob << " pv "; write(ob, bestMove);
    ob << " score " << bestScore;
    ob << " perft " << perftNodes << '\n';
}

void UciSearchInfo::report_perft_divide(const Move& currmove, const Move& bestMove, Score bestScore, index_t currmovenumber, node_count_t perftNodes, node_count_t nodes, const PerftTT& tt) const {
    OUTPUT(ob);
    ob << "info currmovenumber " << currmovenumber;
    ob << " currmove "; write(ob, currmove);
    nps(ob, nodes, tt);
    ob << " pv "; write(ob, bestMove);
    ob << " score " << bestScore;
    ob << " perft " << perftNodes << '\n';
}

#undef OUTPUT
