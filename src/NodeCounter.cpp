#include "NodeCounter.hpp"
#include "SearchControl.hpp"

Control NodeCounter::count(const SearchControl& search) {
    if (tick() == Control::Continue) {
        return Control::Continue;
    }

    if (refreshQuota() == Control::Abort) {
        return Control::Abort;
    }

    if (search.isStopped()) {
        stop();
        return Control::Abort;
    }

    search.readyok();
    return Control::Continue;
}
