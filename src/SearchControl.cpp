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

void SearchControl::infoPosition() const {
    info.position();
}

void SearchControl::go(const SearchLimit& limit) {
    nodeCounter = NodeCounter(limit.nodes);

    info.clear();
    transpositionTable.clearCounter();

    auto duration = getThinkingTime(limit);
    auto searchId = searchThread.start( std::make_unique<NodePerftRoot>(limit, *this) );
    timer.start(duration, searchThread, searchId);
}

Duration SearchControl::getThinkingTime(const SearchLimit& limit) const {
    if (limit.movetime != Duration::zero()) { return limit.movetime; }

    auto moves = limit.movestogo ? limit.movestogo : 60;
    auto average = (limit.time[My] + (moves-1)*limit.inc[My]) / moves;

    return std::min(limit.time[My], average);
}

Control SearchControl::countNode() {
    if (nodeCounter.tick() == Control::Continue) {
        return Control::Continue;
    }

    if (nodeCounter.refreshQuota() == Control::Abort) {
        return Control::Abort;
    }

    if (isStopped()) {
        nodeCounter.stop();
        return Control::Abort;
    }

    info.readyok(nodeCounter.getNodesVisited(), transpositionTable);
    return Control::Continue;
}

void SearchControl::bestmove(const Move& bestMove, Score bestScore) const {
    info.bestmove(bestMove, bestScore, nodeCounter.getNodesVisited(), transpositionTable);
}

void SearchControl::perftDepth(depth_t draft, node_count_t perft, const Move& bestMove, Score bestScore) const {
    info.report_perft_depth(draft, bestMove, bestScore, perft, nodeCounter.getNodesVisited(), transpositionTable);
}

void SearchControl::perftMove(index_t moveCount, const Move& currentMove, node_count_t perft, const Move& bestMove, Score bestScore) const {
    info.report_perft_divide(currentMove, bestMove, bestScore, moveCount, perft, nodeCounter.getNodesVisited(), transpositionTable);
}

void SearchControl::setHash(size_t bytes) {
    transpositionTable.resize(bytes);
}
