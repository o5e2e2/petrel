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

    if (search.isStopped()) {
        nodesLimit = nodes;
        nodesQuota = 0;
        return NodeControl::Abort;
    }

    assert (0 < nodesQuota && nodesQuota <= TickLimit);
    nodes += nodesQuota;
    assert (nodesQuota <= nodes && nodes <= nodesLimit);

    search.readyok();

    --nodesQuota; //count current node
    return NodeControl::Continue;
}
