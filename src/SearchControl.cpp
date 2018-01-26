#include "SearchControl.hpp"
#include "NodePerftRoot.hpp"

SearchControl::SearchControl (SearchInfo& i) : info(i), root(nullptr) { clear(); }

void SearchControl::clear() {
    info.clear();

    if (root) {
        delete root;
        root = nullptr;
    }
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

    root = new NodePerftRoot(searchLimit.getPositionMoves(), *this);
    searchThread.set(root);
    searchSequence = searchThread.start();
    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
