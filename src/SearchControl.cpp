#include "SearchControl.hpp"
#include "NodePerftRoot.hpp"

SearchControl::SearchControl (SearchInfo& i) : info(i) { clear(); }

void SearchControl::clear() {
    info.clear();
}

void SearchControl::newGame() {
    clear();
    transpositionTable.clear();
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

bool SearchControl::countNode() {
    return info.countNode(searchThread);
}

void SearchControl::go() {
    clear();
    info.setNodesLimit( searchLimit.getNodes() );

    auto duration = searchLimit.getThinkingTime();
    auto sequence = searchThread.start( std::make_unique<NodePerftRoot>(searchLimit.getPositionMoves(), *this) );

    searchSequence = sequence;
    timer.start(duration, searchThread, sequence);
}
