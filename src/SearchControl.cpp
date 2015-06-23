#include "SearchControl.hpp"

#include "Search.hpp"
#include "SearchLimit.hpp"
#include "SearchOutput.hpp"
#include "SearchWindow.hpp"
#include "PerftTT.hpp"

SearchControl::SearchControl () : rootWindow(*this) { clear(); }

void SearchControl::clear() {
    info.clear();
    transpositionTable.clear();
    PerftTT::resetAge();
}

void SearchControl::resizeHash(HashMemory::size_t bytes) {
    transpositionTable.resize(bytes);
    PerftTT::resetAge();
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
