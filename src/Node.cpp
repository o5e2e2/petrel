#include "Node.hpp"
#include "SearchControl.hpp"

bool Node::visitChildren() {
    MatrixPiBb& _moves = parent.getMoves();

    for (Pi pi : parent.alivePieces()) {
        Square from = parent.squareOf(pi);

        for (Square to : _moves[pi]) {
            _moves.clear(pi, to);

            CUT (visit(from, to));
        }
    }

    return false;
}
