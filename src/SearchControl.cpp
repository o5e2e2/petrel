#include "SearchControl.hpp"

#include "Search.hpp"
#include "SearchLimit.hpp"
#include "SearchOutput.hpp"
#include "SearchWindow.hpp"

SearchControl::SearchControl () : rootWindow(*this) { clear(); }

void SearchControl::clear() {
    info.clear();
}

void SearchControl::go(SearchOutput& output, const Position& pos, const SearchLimit& searchLimit) {
    info.clear();
    info.out = &output;
    info.nodesLimit = searchLimit.getNodes();

    rootWindow.draft = searchLimit.getDepth();
    rootWindow.searchFn = searchLimit.getDivide()? PerftDivide::perft : Perft::perft;

    sequence = searchThread.start(PerftRoot::perft, pos, rootWindow);
    Timer::start(searchLimit.getThinkingTime(), searchThread, sequence);
}
