#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "UciOutput.hpp"
#include "NodePerft.hpp"
#include "NodeRoot.hpp"

SearchControl::SearchControl (SearchInfo& i) : info(i), root(nullptr) { clear(); }

void SearchControl::clear() {
    info.clear();

    if (root) {
        delete root;
        root = nullptr;
    }
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

bool SearchControl::countNode() {
    return info.countNode(searchThread);
}

void SearchControl::go(const SearchLimit& searchLimit) {
    clear();
    info.setNodesLimit( searchLimit.getNodes() );

    auto draft = searchLimit.getDepth();
    root = new NodeRoot(searchLimit.getPositionMoves(), *this, draft, searchLimit.getDivide());

    searchThread.set(root);
    searchSequence = searchThread.commandRun();
    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
