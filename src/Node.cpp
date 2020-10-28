#include "Node.hpp"
#include "SearchControl.hpp"

 NodeControl Node::beforeVisit() { return control.countNode(); }

 NodeControl Node::visitChildren() {
    auto parentMoves = parent.cloneMoves();

    for (Pi pi : parent[My].alivePieces()) {
        Square from = parent[My].squareOf(pi);

        for (Square to : parentMoves[pi]) {
            RETURN_CONTROL (beforeVisit());
            parentMoves.clear(pi, to);
            RETURN_CONTROL (visit(from, to));
        }
    }

    return NodeControl::Continue;
}
