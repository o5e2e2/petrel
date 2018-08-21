#include "SearchControl.hpp"
#include "NodePerftRoot.hpp"
#include "UciSearchInfo.hpp"
#include "SearchLimit.hpp"

SearchControl::SearchControl (UciSearchInfo& i) : info{i}, searchLimit{} {}

void SearchControl::newGame() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

void SearchControl::go() {
    nodesLimit = searchLimit.getNodesLimit();
    nodes = 0;
    nodesQuota = 0;

    info.clear();
    transpositionTable.clearCounter();

    auto duration = searchLimit.getThinkingTime();
    auto currentSearchId = searchThread.start( std::make_unique<NodePerftRoot>(searchLimit, *this) );
    timer.start(duration, searchThread, currentSearchId);
}

bool SearchControl::refreshQuota() {
    static_assert (TickLimit > 0, "TickLimit should be a positive number");

    assert (nodes >= nodesQuota);
    nodes -= nodesQuota;

    assert (nodesLimit >= nodes);
    auto nodesRemaining = nodesLimit - nodes;

    if (nodesRemaining >= TickLimit) {
        nodesQuota = TickLimit;
    }
    else {
        nodesQuota = static_cast<decltype(nodesQuota)>(nodesRemaining);
        if (nodesQuota == 0) {
            return true;
        }
    }

    if (isStopped()) {
        nodesLimit = nodes;
        nodesQuota = 0;
        return true;
    }

    info.readyok(nodes, tt());

    assert (nodesQuota > 0);
    nodes += nodesQuota;

    --nodesQuota; //count current node
    return false;
}
