#include "NodePerftRootDivide.hpp"
#include "NodePerftRoot.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"

NodePerftRootDivide::NodePerftRootDivide(NodePerftRoot& n) : Node(n) {}

bool NodePerftRootDivide::visit(Square from, Square to) {
    auto& p = static_cast<NodePerftRoot&>(parent);
    auto parentPerftBefore = p.perft;

    switch (p.draft) {
        case 1:
            p.perft += 1;
            break;

        case 2:
            CUT ( NodePerftLeaf(p).visit(from, to) );
            break;

        default:
            CUT ( NodePerftTT(p, p.draft-1).visit(from, to) );
    }

    ++moveCount;
    auto movePerft = p.perft - parentPerftBefore;
    Move move = p.createMove(from, to);
    control.perftMove(moveCount, move, movePerft, p.bestMove, p.bestScore);

    bestScore = Score::Minimum;
    bestMove = {};
    return false;
}
