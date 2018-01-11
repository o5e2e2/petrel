#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "PerftTT.hpp"
#include "PerftRecord.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"

bool NodePerft::visit(Square from, Square to) {
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
            info.inc(PerftNodes, n);
            return false;
        }
    }

    playMove(parent, from, to, zobrist);

    auto perftNodesBefore = info.get(PerftNodes);

    if (draft >= 3) {
        CUT ( NodePerft(*this).visitChildren() );
    }
    else {
        assert (draft == 2);
        CUT ( NodePerftLeaf(*this).visitChildren() );
    }

    auto n = info.get(PerftNodes) - perftNodesBefore;
    PerftTT(origin, hashAge).set(zobrist, draft-2, n);
    info.inc(TT_Written);

    return false;
}
