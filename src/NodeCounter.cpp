#include "NodeCounter.hpp"
#include "SearchControl.hpp"

NodeControl NodeCounter::refreshQuota(const SearchControl& search) {
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
            return NodeControl::Abort;
        }
    }

    assert (nodesQuota > 0);
    nodes += nodesQuota;

    --nodesQuota; //count current node

    if (search.isStopped()) {
        abort();
        return NodeControl::Abort;
    }

    search.readyok();
    return NodeControl::Continue;
}
