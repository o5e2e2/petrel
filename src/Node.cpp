#include "Node.hpp"
#include "SearchControl.hpp"

bool Node::visitChildren() {
    auto& parentMoves = parent.getMoves();

    for (Pi pi : parent.alivePieces()) {
        Square from = parent.squareOf(pi);

        for (Square to : parentMoves[pi]) {
            parentMoves.clear(pi, to);

            CUT (visit(from, to));
        }
    }

    return false;
}
