#include "SearchControl.hpp"
#include "NodePerftRoot.hpp"
#include "NodeAbRoot.hpp"
#include "UciSearchInfo.hpp"
#include "SearchLimit.hpp"

SearchControl::SearchControl (UciSearchInfo& i) : info{i}, pvMoves{} {}

void SearchControl::newGame() {
    transpositionTable.clear();
    newSearch();
}

void SearchControl::newSearch() {
    newIteration();
    transpositionTable.clearCounter();
}

void SearchControl::newIteration() {
    pvMoves.clear();
    transpositionTable.nextAge();
}

void SearchControl::go(const SearchLimit& limit) {
    newSearch();

    nodeCounter = NodeCounter(limit.nodes);
    auto duration = limit.getThinkingTime();

    auto searchId = searchThread.start( limit.isPerft
        ? static_cast<std::unique_ptr<Node>>(std::make_unique<NodePerftRoot>(limit, *this))
        : static_cast<std::unique_ptr<Node>>(std::make_unique<NodeAbRoot>(limit, *this))
    );

    timer.start(duration, searchThread, searchId);
}

NodeControl SearchControl::countNode() {
    return nodeCounter.count(*this);
}

void SearchControl::createPv(const Move& move) {
    pvMoves(ply-1, move);
}

void SearchControl::uciok() const {
    info.uciok(transpositionTable.getInfo());
}

void SearchControl::isready() const {
    info.isready(!isBusy());
}

void SearchControl::infoPosition() const {
    info.position();
}

void SearchControl::readyok() const{
    info.readyok(nodeCounter, transpositionTable);
}

void SearchControl::bestmove(Score bestScore) const {
    info.bestmove(pvMoves, bestScore, nodeCounter, transpositionTable);
}

void SearchControl::infoDepth(ply_t draft, Score bestScore) const {
    info.report_depth(draft, pvMoves, bestScore, nodeCounter, transpositionTable);
}

void SearchControl::infoPerftDepth(ply_t draft, node_count_t perft, Score bestScore) const {
    info.report_perft_depth(draft, pvMoves, bestScore, perft, nodeCounter, transpositionTable);
}

void SearchControl::infoPerftMove(index_t moveCount, const Move& currentMove, node_count_t perft, Score bestScore) const {
    info.report_perft_divide(currentMove, pvMoves, bestScore, moveCount, perft, nodeCounter, transpositionTable);
}

void SearchControl::setHash(size_t bytes) {
    transpositionTable.resize(bytes);
}
