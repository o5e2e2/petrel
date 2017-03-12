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
    rootWindow.searchFn = searchLimit.getDivide()? PerftDivide::perft : Perft::perft;

    assert (searchThread.isReady());
    searchThread.set(PerftRoot::perft, rootMoves, rootWindow);
    searchSequence = searchThread.commandRun();
    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
