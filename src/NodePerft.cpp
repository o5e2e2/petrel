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
            RETURN_CONTROL ( NodePerftTT(*this).visitChildren() );
    }

    updateParentPerft();

    return NodeControl::Continue;
}
