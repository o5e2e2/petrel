#include "SearchControl.hpp"

#include "Node.hpp"
#include "SearchLimit.hpp"
#include "SearchOutput.hpp"

SearchControl::SearchControl () { clear(); }

void SearchControl::clear() {
    transpositionTable.clear();
    info.clear();
}

void SearchControl::go(SearchOutput& output, const Position& startPosition, const SearchLimit& goLimit) {
    info.clear();
    info.out = &output;
    info.nodesLimit = goLimit.getNodes();

    auto searchFn = goLimit.getDivide()? PerftDivideRoot::perft : PerftRoot::perft;
    sequence = searchThread.start(searchFn, this, &startPosition, goLimit.getDepth());
    Timer::start(goLimit.getThinkingTime(), searchThread, sequence);
}
