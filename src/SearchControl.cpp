#include "SearchControl.hpp"
#include "NodePerftRoot.hpp"
#include "UciSearchInfo.hpp"
#include "SearchLimit.hpp"

SearchControl::SearchControl (UciSearchInfo& i) : info{i} {}

void SearchControl::newGame() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

void SearchControl::setHash(io::istream& command) {
    if (!this->isReady()) {
        io::fail_rewind(command);
        return;
    }

    size_t quantity = 0;
    command >> quantity;
    if (!command) {
        io::fail_rewind(command);
        return;
    }

    io::char_type unit = 'm';
    command >> unit;

    switch (std::tolower(unit)) {
        case 't': quantity *= 1024;
        case 'g': quantity *= 1024;
        case 'm': quantity *= 1024;
        case 'k': quantity *= 1024;
        case 'b': break;

        default: {
            io::fail_rewind(command);
            return;
        }
    }

    tt().resize(quantity);
}

void SearchControl::go(const SearchLimit& searchLimit) {
    nodes = 0;
    nodesLimit = searchLimit.getNodes();
    nodesQuota = 0;
    info.clear();
    transpositionTable.clearCounter();

    auto duration = searchLimit.getThinkingTime();
    auto sequence = searchThread.start( std::make_unique<NodePerftRoot>(searchLimit, *this) );

    searchSequence = sequence;
    timer.start(duration, searchThread, sequence);
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
