#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "UciOutput.hpp"

SearchControl::SearchControl (SearchInfo& i) : info(i), rootWindow(*this) { clear(); }

void SearchControl::clear() {
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

    rootWindow.draft = searchLimit.getDepth();
    auto rootSearch = (rootWindow.draft > 0)? Perft::perftRoot : Perft::perftId;
    rootWindow.searchFn = searchLimit.getDivide()? Perft::divide : Perft::perft;

    searchThread.set(rootSearch, rootMoves, rootWindow);
    searchSequence = searchThread.commandRun();

    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
