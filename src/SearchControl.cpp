#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "UciOutput.hpp"
#include "NodePerft.hpp"
#include "NodeRoot.hpp"

SearchControl::SearchControl (SearchInfo& i) : info(i), root(nullptr), child(nullptr) { clear(); }

void SearchControl::clear() {
    if (root) {
        delete root;
    }
    if (child) {
        delete child;
    }
    info.clear();
    transpositionTable.clear();
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

bool SearchControl::checkQuota() {
    return info.checkQuota(searchThread);
}

void SearchControl::go(const PositionMoves& rootMoves, const SearchLimit& searchLimit) {
    info.clear();
    info.setNodesLimit( searchLimit.getNodes() );

    auto draft = searchLimit.getDepth();
    root = new NodeRoot(rootMoves, *this, draft, searchLimit.getDivide());
    child = new NodePerft(*root);

    searchThread.set(root, child);
    searchSequence = searchThread.commandRun();

    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
