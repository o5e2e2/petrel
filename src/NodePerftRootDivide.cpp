#include "NodePerftRootDivide.hpp"
#include "NodePerftRoot.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"

NodePerftRootDivide::NodePerftRootDivide(NodePerftRoot& n) : Node(n) {}

Control NodePerftRootDivide::visit(Square from, Square to) {
    auto& p = static_cast<NodePerftRoot&>(parent);
    auto parentPerftBefore = p.perft;

    switch (p.draft) {
        case 1:
            p.perft += 1;
            break;

        case 2:
            RETURN_CONTROL ( NodePerftLeaf(p).visit(from, to) );
            break;

        default:
            RETURN_CONTROL ( NodePerftTT(p, p.draft-1).visit(from, to) );
    }

    ++moveCount;
    auto movePerft = p.perft - parentPerftBefore;
    Move move = p.createMove(from, to);
    control.infoPerftMove(moveCount, move, movePerft, p.bestMove, p.bestScore);

    bestScore = Score::Minimum;
    bestMove = {};
    return Control::Continue;
}
