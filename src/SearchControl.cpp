#include "SearchControl.hpp"
#include "NodePerftRoot.hpp"
#include "UciSearchInfo.hpp"
#include "SearchLimit.hpp"

SearchControl::SearchControl (UciSearchInfo& i) : info{i} {}

void SearchControl::newGame() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

void SearchControl::uciok() const {
    info.uciok(transpositionTable.getInfo());
}

void SearchControl::readyok() const {
    info.isready(!isBusy());
}

void SearchControl::go(const SearchLimit& searchLimit) {
    nodesLimit = searchLimit.getNodesLimit();
    nodes = 0;
    nodesQuota = 0;

    info.clear();
    transpositionTable.clearCounter();

    auto duration = searchLimit.getThinkingTime();
    auto searchId = searchThread.start( std::make_unique<NodePerftRoot>(searchLimit, *this) );
    timer.start(duration, searchThread, searchId);
}

void SearchControl::bestmove(const Move& bestMove, Score bestScore) const {
    info.bestmove(bestMove, bestScore, getNodesVisited(), transpositionTable);
}

void SearchControl::perftDepth(depth_t draft, node_count_t perft, const Move& bestMove, Score bestScore) const {
    info.report_perft_depth(draft, bestMove, bestScore, perft, getNodesVisited(), transpositionTable);
}

void SearchControl::perftMove(index_t moveCount, const Move& currentMove, node_count_t perft, const Move& bestMove, Score bestScore) const {
    info.report_perft_divide(currentMove, bestMove, bestScore, moveCount, perft, getNodesVisited(), transpositionTable);
}

void SearchControl::setHash(size_t bytes) {
    transpositionTable.resize(bytes);
}

bool SearchControl::refreshQuota() {
    static_assert (TickLimit > 0, "TickLimit should be a positive number");

    assert (nodes >= nodesQuota);
    nodes -= nodesQuota;

    assert (nodesLimit >= nodes);
    auto nodesRemaining = nodesLimit - nodes;

    if (nodesRemaining >= TickLimit) {
        nodesQuota = TickLimit;
    }
    else {
        nodesQuota = static_cast<decltype(nodesQuota)>(nodesRemaining);
        if (nodesQuota == 0) {
            return true;
        }
    }

    if (isStopped()) {
        nodesLimit = nodes;
        nodesQuota = 0;
        return true;
    }

    info.readyok(nodes, transpositionTable);

    assert (nodesQuota > 0);
    nodes += nodesQuota;

    --nodesQuota; //count current node
    return false;
}
