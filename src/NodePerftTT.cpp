#include "NodePerftTT.hpp"
#include "PerftTT.hpp"
#include "SearchControl.hpp"

bool NodePerftTT::visit(Square from, Square to) {
    auto& p = static_cast<NodePerft&>(parent);

    zobrist = p.createZobrist(from, to);

    {
        auto n = control.tt().get(zobrist, draft-2);

        if (n != NodeCountNone) {
            perft = n;
            updateParentPerft();
            return false;
        }
    }

    auto parentPerftBefore = p.perft;
    CUT ( NodePerft::visit(from, to) );
    auto n = p.perft - parentPerftBefore;

    control.tt().set(zobrist, draft-2, n);

    return false;
}
