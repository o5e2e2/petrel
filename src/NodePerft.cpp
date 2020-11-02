#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"
#include "SearchControl.hpp"

NodeControl NodePerft::visit(Square from, Square to) {
    playMove(parent, from, to, getZobrist());

    switch (draft) {
        case 2:
            RETURN_CONTROL ( NodePerftLeaf(*this).visitChildren() );
            break;

        default:
            assert (draft >= 3);
            RETURN_CONTROL ( NodePerft(*this).visitChildren() );
    }

    updateParentPerft();

    auto& p = static_cast<NodePerft&>(parent);
    if (-bestScore > p.bestScore) {
        p.bestScore = -bestScore;
        control.createPv(p.createMove(from, to));
    }

    bestScore = Score::Minimum;
    return NodeControl::Continue;
}
