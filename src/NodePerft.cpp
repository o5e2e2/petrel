#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"

bool NodePerft::visit(Square from, Square to) {
    playMove(parent, from, to, getZobrist());

    switch (draft) {
        case 2:
            CUT ( NodePerftLeaf(*this).visitChildren() );
            break;

        default:
            assert (draft >= 3);
            CUT ( NodePerftTT(*this, draft-1).visitChildren() );
    }

    updateParentPerft();

    auto& p = static_cast<NodePerft&>(parent);
    if (-bestScore > p.bestScore) {
        p.bestScore = -bestScore;
        p.bestMove = p.createMove(from, to);
    }

    bestScore = Score::Minimum;
    bestMove = {};
    return false;
}
