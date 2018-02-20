#include "SearchInfo.hpp"
#include "SearchControl.hpp"

void SearchInfo::clear() {
    nodes = 0;
    nodesLimit = NODE_COUNT_MAX;
    nodesQuota = 0;
    _v = {0, 0, 0, 0, 0};
}

//callbacks from search thread
bool SearchInfo::refreshQuota(const SearchControl& searchControl) {
    static_assert (TickLimit > 0, "TickLimit should be positive number");

    assert (nodesQuota < 0 || nodes >= static_cast<decltype(nodes)>(nodesQuota));
    nodes -= nodesQuota;

    assert (nodesLimit >= nodes);
    auto nodesRemaining = nodesLimit - nodes;

    if (nodesRemaining >= TickLimit) {
        nodesQuota = static_cast<decltype(nodesQuota)>(TickLimit);
    }
    else {
        nodesQuota = static_cast<decltype(nodesQuota)>(nodesRemaining);
        if (nodesRemaining == 0) {
            return true;
        }
    }

    if (searchControl.isStopped()) {
        nodesQuota = 0;
        nodesLimit = nodes;
        return true;
    }

    this->readyok();

    assert (nodesQuota > 0);
    nodes += static_cast<decltype(nodes)>(nodesQuota);

    --nodesQuota; //count current node
    return false;
}
