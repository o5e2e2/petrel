#include "NodePerftTT.hpp"
#include "PerftTT.hpp"
#include "SearchControl.hpp"

Control NodePerftTT::visit(Square from, Square to) {
    auto& p = static_cast<NodePerft&>(parent);

    setZobrist(p, from, to);

    {
        auto n = control.tt().get(getZobrist(), draft-2);

        if (n != NodeCountNone) {
            perft = n;
            updateParentPerft();
            return Control::Continue;
        }
    }

    auto parentPerftBefore = p.perft;
    RETURN_CONTROL ( NodePerft::visit(from, to) );
    auto n = p.perft - parentPerftBefore;

    control.tt().set(getZobrist(), draft-2, n);

    return Control::Continue;
}
