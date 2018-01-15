#include "NodePerftRootDivide.hpp"
#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
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
            CUT ( NodePerft(*this).visit(from, to) );
    }

    Move move = getSide(My).createMove(from, to);
    control.info.report_perft_divide(move);

    return false;
}
