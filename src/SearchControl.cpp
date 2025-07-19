#include "SearchControl.hpp"
#include "NodeAbRoot.hpp"
#include "NodePerftRoot.hpp"
#include "SearchLimit.hpp"
#include "OutputBuffer.hpp"

#define OUTPUT(ob) OutputBuffer<decltype(outLock)> ob(out, outLock)

void SearchControl::newGame() {
    tt.clear();
    newSearch();
}

void SearchControl::newSearch() {
    pvMoves.clear();
    tt.clearCounter();
    lastInfoNodes = 0;
    fromSearchStart = {};
}

void SearchControl::newIteration() {
    tt.nextAge();
}

void SearchControl::go(const SearchLimit& limit) {
    nodeCounter = { limit.nodes };
    auto searchId = searchThread.start(static_cast<std::unique_ptr<Node>>(
        std::make_unique<NodeAbRoot>(limit, *this)
    ));

    if (!limit.isInfinite) {
        auto duration = limit.getThinkingTime();
        timer.start(duration, searchThread, searchId);
    }
}

void SearchControl::goPerft(Ply depth, bool isDivide) {
    nodeCounter = {};
    searchThread.start(static_cast<std::unique_ptr<Node>>(
        std::make_unique<NodePerftRoot>(position, *this, depth, isDivide)
    ));
}

NodeControl SearchControl::countNode() {
    return nodeCounter.count(*this);
}

void SearchControl::uciok() const {
    bool isChess960 = position.isChess960();

    OUTPUT(ob);
    ob << "id name petrel\n";
    ob << "id author Aleks Peshkov\n";
    ob << "option name UCI_Chess960 type check default " << (isChess960 ? "true" : "false") << '\n';
    ob << "option name Hash type spin min 0 max " << mebi(tt.getInfo().maxSize) << " default " << mebi(tt.getInfo().currentSize) << '\n';
    ob << "uciok\n";
}

void SearchControl::isready() const {
    OUTPUT(ob);
    if (!isBusy()) {
        isreadyWaiting = false;
        ob << "readyok\n";
    }
    else {
        isreadyWaiting = true;
    }
}

void SearchControl::infoPosition() const {
    OUTPUT(ob);
    ob << "info fen " << position << '\n';
    ob << "info " << position.evaluate() << '\n';
}

void SearchControl::readyok() const {
    if (isreadyWaiting) {
        std::ostringstream ob;
        info_nps(ob, nodeCounter);
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

void SearchControl::bestmove() const {
    OUTPUT(ob);
    if (lastInfoNodes != nodeCounter) {
        ob << "info"; nps(ob, nodeCounter) << '\n';
    }
    ob << "bestmove " << pvMoves[0] << '\n';
}

void SearchControl::infoIterationEnd(Ply draft) const {
    OUTPUT(ob);
    ob << "info depth " << draft; nps(ob, nodeCounter) << '\n';
}

void SearchControl::infoNewPv(Ply draft, Score score) const {
    OUTPUT(ob);
    ob << "info depth " << draft; nps(ob, nodeCounter) << score << " pv" << pvMoves << '\n';
}

void SearchControl::perft_depth(Ply draft, node_count_t perft) const {
    OUTPUT(ob);
    ob << "info depth " << draft << " perft " << perft; nps(ob, nodeCounter) << '\n';
}

void SearchControl::perft_currmove(index_t moveCount, const Move& currentMove, node_count_t perft) const {
    OUTPUT(ob);
    ob << "info currmovenumber " << moveCount << " currmove " << currentMove << " perft " << perft; nps(ob, nodeCounter) << '\n';
}

void SearchControl::perft_finish() const {
    if (lastInfoNodes != nodeCounter) {
        OUTPUT(ob);
        ob << "info"; nps(ob, nodeCounter) << '\n';
    }
    OUTPUT(ob);
    ob << "bestmove 0000\n";
}

void SearchControl::setHash(size_t bytes) {
    tt.resize(bytes);
}

ostream& SearchControl::nps(ostream& o, node_count_t nodes) const {
    if (lastInfoNodes == nodes) {
        return o;
    }
    lastInfoNodes = nodes;

    o << " nodes " << nodes;

    auto duration = fromSearchStart.getDuration();
    if (duration >= Milliseconds{1}) {
        o << " time " << milliseconds(duration);

        if (duration >= Milliseconds{20}) {
            o << " nps " << ::nps(nodes, duration);
        }
    }

    auto& counter = tt.getCounter();
    if (counter.reads > 0) {
        o << " hwrites " << counter.writes;
        o << " hhits " << counter.hits;
        o << " hreads " << counter.reads;
        o << " hhitratio " << permil(counter.hits, counter.reads);
    }
    return o;
}

ostream& SearchControl::info_nps(ostream& o, node_count_t nodes) const {
    std::ostringstream buffer;
    nps(buffer, nodes);

    if (!buffer.str().empty()) {
        o << "info" << buffer.str() << '\n';
    }
    return o;
}

#undef OUTPUT
