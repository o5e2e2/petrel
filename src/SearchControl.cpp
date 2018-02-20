#include "SearchControl.hpp"
#include "NodePerftRoot.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"

SearchControl::SearchControl (SearchInfo& i) : info{i} {}

void SearchControl::newGame() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

bool SearchControl::countNode() {
    return info.countNode(*this);
}

void SearchControl::go(const SearchLimit& searchLimit) {
    info.searchStarted();
    info.setNodesLimit( searchLimit.getNodes() );

    auto duration = searchLimit.getThinkingTime();
    auto sequence = searchThread.start( std::make_unique<NodePerftRoot>(searchLimit, *this) );

    searchSequence = sequence;
    timer.start(duration, searchThread, sequence);
}
