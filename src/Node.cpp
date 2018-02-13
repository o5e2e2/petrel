#include "Node.hpp"
#include "SearchControl.hpp"

Node::Node (Node& n) : PositionMoves(), parent(n), control(n.control) {}
Node::Node (const PositionMoves& p, SearchControl& c) : PositionMoves(p), parent(*this), control(c) {}
Node::Node (const PositionMoves& p, Node& n) : PositionMoves(p), parent(n), control(n.control) {}

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
