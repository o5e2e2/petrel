#include "SearchControl.hpp"

#include "Search.hpp"
#include "SearchLimit.hpp"
#include "SearchOutput.hpp"
#include "SearchWindow.hpp"

SearchControl::SearchControl () : rootWindow(*this) { clear(); }

void SearchControl::clear() {
    info.clear();
}

void SearchControl::go(SearchOutput& output, const Position& pos, const SearchLimit& goLimit) {
    info.clear();
    info.out = &output;
    info.nodesLimit = goLimit.getNodes();

    rootWindow.draft = goLimit.getDepth();
    rootWindow.searchFn = goLimit.getDivide()? PerftDivide::perft : Perft::perft;

    sequence = searchThread.start(PerftRoot::perft, pos, rootWindow);
    Timer::start(goLimit.getThinkingTime(), searchThread, sequence);
}
