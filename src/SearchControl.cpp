#include "SearchControl.hpp"

#include "Search.hpp"
#include "SearchLimit.hpp"
#include "SearchOutput.hpp"
#include "SearchWindow.hpp"

SearchControl::SearchControl () { clear(); }

void SearchControl::clear() {
    transpositionTable.clear();
    info.clear();
}

void SearchControl::go(SearchOutput& output, const Position& startPosition, const SearchLimit& goLimit) {
    info.clear();
    info.out = &output;
    info.nodesLimit = goLimit.getNodes();

    window.draft = goLimit.getDepth();
    window.searchFn = goLimit.getDivide()? PerftDivide::perft : Perft::perft;

    sequence = searchThread.start(PerftRoot::perft, startPosition, *this, window);
    Timer::start(goLimit.getThinkingTime(), searchThread, sequence);
}
