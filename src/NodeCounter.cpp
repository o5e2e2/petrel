#include "NodeCounter.hpp"
#include "SearchControl.hpp"

NodeControl NodeCounter::count(const SearchControl& search) {
    if (tick() == NodeControl::Continue) {
        return NodeControl::Continue;
    }

    if (refreshQuota() == NodeControl::Abort) {
        return NodeControl::Abort;
    }

    if (search.isStopped()) {
        stop();
        return NodeControl::Abort;
    }

    search.readyok();
    return NodeControl::Continue;
}
