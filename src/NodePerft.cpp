#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftTT.hpp"

bool NodePerft::visit(Square from, Square to) {
    playMove(parent, from, to, zobrist);

    if (draft == 2) {
        CUT ( NodePerftLeaf(*this).visitChildren() );
    }
    else {
        assert (draft >= 3);
        CUT ( NodePerftTT(*this, draft-1).visitChildren() );
    }

    updateParentPerft();
    return false;
}
