#include "NodeCounter.hpp"
#include "SearchControl.hpp"

NodeControl NodeCounter::refreshQuota(const SearchControl& search) {
    assert (nodesQuota <= nodes && nodes <= nodesLimit);

    nodes -= nodesQuota;

    auto nodesRemaining = nodesLimit - nodes;
    if (nodesRemaining >= TickLimit) {
        nodesQuota = TickLimit;
    }
    else {
        nodesQuota = static_cast<decltype(nodesQuota)>(nodesRemaining);
        if (nodesQuota == 0) {
            return NodeControl::Abort;
        }
    }

    assert (0 < nodesQuota && nodesQuota <= TickLimit);
    nodes += nodesQuota;

    --nodesQuota; //count current node

    if (search.isStopped()) {
        abort();
        return NodeControl::Abort;
    }

    search.readyok();
    return NodeControl::Continue;
}
