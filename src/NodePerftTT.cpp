#include "NodePerftTT.hpp"
#include "PerftTT.hpp"
#include "SearchControl.hpp"
#include "UciSearchInfo.hpp"

bool NodePerftTT::visit(Square from, Square to) {
    auto& p = static_cast<NodePerft&>(parent);
    auto& info = control.info;

    zobrist = p.createZobrist(from, to);
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

    auto parentPerftBefore = p.perft;
    CUT ( NodePerft::visit(from, to) );
    auto n = p.perft - parentPerftBefore;

    PerftTT(origin, hashAge).set(zobrist, draft-2, n);
    info.inc(TT_Written);

    return false;
}
