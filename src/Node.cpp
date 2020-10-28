#include "Node.hpp"
#include "SearchControl.hpp"

 Control Node::beforeVisit() { return control.countNode(); }

 Control Node::visitChildren() {
    auto parentMoves = parent.cloneMoves();

    for (Pi pi : parent[My].alivePieces()) {
        Square from = parent[My].squareOf(pi);

        for (Square to : parentMoves[pi]) {
            RETURN_CONTROL (beforeVisit());
            parentMoves.clear(pi, to);
            RETURN_CONTROL (visit(from, to));
        }
    }

    return Control::Continue;
}
