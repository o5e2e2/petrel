#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "PerftTT.hpp"
#include "PerftRecord.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"

bool NodePerft::visit(Square from, Square to) {
    auto& info = control.info;

    CUT ( control.checkQuota() );
    control.info.decrementQuota();

    zobrist = parent.createZobrist(from, to);
    auto origin = control.tt().lookup(zobrist);

    {
        auto n = PerftTT(origin, control.tt().getAge()).get(zobrist, draft);
        info.inc(TT_Tried);

        if (n != NODE_COUNT_NONE) {
            info.inc(TT_Hit);
            info.inc(PerftNodes, n);
            return false;
        }
    }

    playMove(parent, from, to, zobrist);

    auto perftNodesBefore = info.get(PerftNodes);

    if (draft >= 2) {
        NodePerft child(*this);
        CUT (visitChildren(child));
    }
    else {
        NodePerftLeaf child(*this);
        CUT (visitChildren(child));
    }

    auto n = info.get(PerftNodes) - perftNodesBefore;
    PerftTT(origin, control.tt().getAge()).set(zobrist, draft, n);
    info.inc(TT_Written);

    return false;
}
