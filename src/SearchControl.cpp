#include "SearchControl.hpp"

#include "Search.hpp"
#include "SearchLimit.hpp"
#include "SearchOutput.hpp"
#include "SearchWindow.hpp"
#include "HashBucket.hpp"

SearchControl::SearchControl () : rootWindow(*this) { clear(); }

void SearchControl::clear() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::resizeHash(HashMemory::size_t bytes) {
    transpositionTable.resize(bytes);
}

void SearchControl::go(SearchOutput& output, const Position& pos, const SearchLimit& searchLimit) {
    info.clear();
    info.out = &output;
    info.nodesLimit = searchLimit.getNodes();

    rootWindow.draft = searchLimit.getDepth();
    rootWindow.searchFn = searchLimit.getDivide()? PerftDivide::perft : Perft::perft;

    searchThread.set(PerftRoot::perft, pos, rootWindow);
    searchSequence = Timer::start(searchThread, searchLimit.getThinkingTime());
}
