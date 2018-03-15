#include "NodePerftTT.hpp"
#include "PerftTT.hpp"
#include "SearchControl.hpp"

bool NodePerftTT::visit(Square from, Square to) {
    auto& p = static_cast<NodePerft&>(parent);

    zobrist = p.createZobrist(from, to);
    auto origin = control.tt().prefetch(zobrist);
    auto hashAge = control.tt().getAge();

    {
        auto n = PerftTT(origin, hashAge).get(zobrist, draft-2);
        control.tt().countRead();

        if (n != NODE_COUNT_NONE) {
            control.tt().countHit();
            perft = n;
            updateParentPerft();
            return false;
        }
    }

    auto parentPerftBefore = p.perft;
    CUT ( NodePerft::visit(from, to) );
    auto n = p.perft - parentPerftBefore;

    PerftTT(origin, hashAge).set(zobrist, draft-2, n);
    control.tt().countWrite();

    return false;
}
