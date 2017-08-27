#include "Node.hpp"

bool Node::visitChildren(Node& child) {
    MatrixPiBb& _moves = getMoves();

    for (Pi pi : alivePieces()) {
        Square from = squareOf(pi);

        for (Square to : _moves[pi]) {
            _moves.clear(pi, to);

            CUT (child.visit(from, to));
        }
    }

    return false;
}
