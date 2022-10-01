#include "NodePerftTT.hpp"
#include "PerftTT.hpp"
#include "SearchControl.hpp"

NodeControl NodePerftTT::visit(Square from, Square to) {
    auto& p = static_cast<NodePerft&>(parent);

    assert (draft >= 2);
    setZobrist(p, from, to);

    {
        auto n = control.tt().get(getZobrist(), draft-2);

        if (n != NodeCountNone) {
            perft = n;
            updateParentPerft();
            return NodeControl::Continue;
        }
    }

    auto parentPerftBefore = p.perft;
    RETURN_IF_ABORT ( NodePerft::visit(from, to) );
    auto n = p.perft - parentPerftBefore;

    control.tt().set(getZobrist(), draft-2, n);

    return NodeControl::Continue;
}
