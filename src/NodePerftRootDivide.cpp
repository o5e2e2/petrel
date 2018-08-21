#include "NodePerftRootDivide.hpp"
#include "NodePerftRoot.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"
#include "UciSearchInfo.hpp"

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
    auto n = p.perft - parentPerftBefore;
    Move move = p.createMove(from, to);
    control.info.report_perft_divide(move, moveCount, n, control.getNodesVisited(), control.tt());

    return false;
}
