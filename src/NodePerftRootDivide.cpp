#include "NodePerftRootDivide.hpp"
#include "NodePerftRoot.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"
#include "Move.hpp"

NodePerftRootDivide::NodePerftRootDivide(NodePerftRoot& p)
    : NodePerft(p, p), moveCount(0) {}

bool NodePerftRootDivide::visit(Square from, Square to) {
    switch (draft) {
        case 1:
            perft += 1;
            break;

        case 2:
            CUT ( NodePerftLeaf(*this).visit(from, to) );
            break;

        default:
            CUT ( NodePerftTT(*this, draft-1).visit(from, to) );
    }

    ++moveCount;
    Move move = createMove(from, to);
    control.info.report_perft_divide(move, moveCount, perft);
    count();
    return false;
}
