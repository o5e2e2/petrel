#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "PositionMoves.hpp"
#include "Timer.hpp"

SearchControl::SearchControl (SearchOutput& out) : info(out), rootWindow(*this) { clear(); }

void SearchControl::clear() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::resizeHash(HashMemory::size_t bytes) {
    info.clear();
    transpositionTable.resize(bytes);
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

void SearchControl::go(const PositionMoves& rootMoves, const SearchLimit& searchLimit) {
    info.clear();
    info.nodesLimit = searchLimit.getNodes();

    rootWindow.draft = searchLimit.getDepth();
    auto rootSearch = (rootWindow.draft > 0)? Perft::perftRoot : Perft::perftId;
    rootWindow.searchFn = searchLimit.getDivide()? Perft::divide : Perft::perft;

    searchThread.set(rootSearch, rootMoves, rootWindow);
    searchSequence = searchThread.commandRun();

    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
