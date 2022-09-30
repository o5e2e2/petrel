#include "NodePerftRootDivide.hpp"
#include "NodePerftRoot.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"

NodePerftRootDivide::NodePerftRootDivide(NodePerftRoot& n) : NodePerft(n) {}

NodeControl NodePerftRootDivide::visit(Square from, Square to) {
    auto& p = static_cast<NodePerftRoot&>(parent);
    auto perftBeforeMove = p.perft;

    switch (p.draft) {
        case 1:
            p.perft += 1;
            break;

        case 2:
            RETURN_IF_ABORT ( NodePerftLeaf(p).visit(from, to) );
            break;

        default:
            assert (p.draft > 2);
            RETURN_IF_ABORT ( NodePerftTT(p).visit(from, to) );
    }

    ++moveCount;
    auto movePerft = p.perft - perftBeforeMove;
    Move move = p.createMove(from, to);
    control.perft_currmove(moveCount, move, movePerft);

    return NodeControl::Continue;
}
