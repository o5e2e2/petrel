#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "PositionMoves.hpp"
#include "Timer.hpp"

SearchControl::SearchControl () : rootWindow(*this) { clear(); }

void SearchControl::clear() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::resizeHash(HashMemory::size_t bytes) {
    info.clear();
    transpositionTable.resize(bytes);
}

void SearchControl::nextIteration() {
    info.clearNodes();
    transpositionTable.nextAge();
}

void SearchControl::go(SearchOutput& output, const PositionMoves& rootMoves, const SearchLimit& searchLimit) {
    info.clear();
    info.out = &output;
    info.nodesLimit = searchLimit.getNodes();

    rootWindow.draft = searchLimit.getDepth();
    rootWindow.searchFn = searchLimit.getDivide()? PerftDivide::perft : Perft::perft;

    assert (searchThread.isReady());
    searchThread.set(PerftRoot::perft, rootMoves.getPos(), rootWindow);
    searchSequence = searchThread.commandRun();
    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
