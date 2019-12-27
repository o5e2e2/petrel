#include "Node.hpp"
#include "SearchControl.hpp"

bool Node::visitChildren() {
    auto parentMoves = parent.cloneMoves();

    for (Pi pi : parent[My].alivePieces()) {
        Square from = parent[My].squareOf(pi);

        for (Square to : parentMoves[pi]) {
            CUT (control.countNode());
            parentMoves.clear(pi, to);
            CUT (visit(from, to));
        }
    }

    return false;
}
