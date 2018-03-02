#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"

bool NodePerft::visit(Square from, Square to) {
    playMove(parent, from, to, zobrist);

    switch (draft) {
        case 2:
            CUT ( NodePerftLeaf(*this).visitChildren() );
            break;

        default:
            assert (draft >= 3);
            CUT ( NodePerftTT(*this, draft-1).visitChildren() );
    }

    updateParentPerft();
    return false;
}
