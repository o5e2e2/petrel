#include "NodePerftTT.hpp"
#include "PerftTT.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"

bool NodePerftTT::visit(Square from, Square to) {
    auto& info = control.info;
    CUT ( control.countNode() );

    zobrist = parent.createZobrist(from, to);
    auto origin = control.tt().prefetch(zobrist);
    auto hashAge = control.tt().getAge();

    {
        auto n = PerftTT(origin, hashAge).get(zobrist, draft-2);
        info.inc(TT_Tried);

        if (n != NODE_COUNT_NONE) {
            info.inc(TT_Hit);
            perft = n;
            updateParentPerft();
            return false;
        }
    }

    auto parentPerftBefore = static_cast<NodePerft&>(parent).perft;
    CUT ( NodePerft::visit(from, to) );
    auto n = static_cast<NodePerft&>(parent).perft - parentPerftBefore;

    PerftTT(origin, hashAge).set(zobrist, draft-2, n);
    info.inc(TT_Written);

    return false;
}
