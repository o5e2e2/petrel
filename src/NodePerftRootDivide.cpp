#include "NodePerftRootDivide.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"
#include "Move.hpp"

bool NodePerftRootDivide::visit(Square from, Square to) {
    switch (draft) {
        case 1:
            control.info.inc(PerftNodes, 1);
            break;

        case 2:
            CUT ( NodePerftLeaf(*this).visit(from, to) );
            break;

        default:
            CUT ( NodePerftTT(*this, draft-1).visit(from, to) );
    }

    Move move = createMove(from, to);
    control.info.report_perft_divide(move);
    return false;
}
