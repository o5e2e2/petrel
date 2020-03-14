#include "Node.hpp"
#include "SearchControl.hpp"

Control Node::visitChildren() {
    auto parentMoves = parent.cloneMoves();

    for (Pi pi : parent[My].alivePieces()) {
        Square from = parent[My].squareOf(pi);

        for (Square to : parentMoves[pi]) {
            RETURN_CONTROL (control.countNode());
            parentMoves.clear(pi, to);
            RETURN_CONTROL (visit(from, to));
        }
    }

    return Control::Continue;
}
