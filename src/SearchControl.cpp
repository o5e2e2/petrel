#include "SearchControl.hpp"
#include "SearchLimit.hpp"
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

void SearchControl::go(SearchOutput& output, const Position& pos, const SearchLimit& l) {
    searchLimit = l;

    info.clear();
    info.out = &output;
    info.nodesLimit = searchLimit.getNodes();

    rootWindow.draft = searchLimit.getDepth();
    rootWindow.searchFn = searchLimit.getDivide()? PerftDivide::perft : Perft::perft;

    searchThread.set(PerftRoot::perft, pos, rootWindow);
    searchSequence = Timer::start(searchThread, searchLimit.getThinkingTime());
}
