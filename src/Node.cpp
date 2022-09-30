#include "Node.hpp"
#include "SearchControl.hpp"

Node::Node (Node& n, ply_t r)
    : PositionMoves{}
    , parent{n}
    , control{n.control}
    , draft{n.draft - r}
    {}

namespace {
    class SearchPly {
        SearchControl& control;
    public:
        SearchPly (Node& n) : control{n.control} { ++control.ply; }
        ~SearchPly() { --control.ply; }
    };
}

NodeControl Node::beforeVisit() { return control.countNode(); }

NodeControl Node::visitChildren() {
    SearchPly ply(*this);

    auto parentMoves = parent.cloneMoves();

    for (Pi pi : parent[My].alivePieces()) {
        Square from = parent[My].squareOf(pi);

        for (Square to : parentMoves[pi]) {
            RETURN_IF_ABORT (beforeVisit());
            parentMoves.clear(pi, to);
            RETURN_IF_ABORT (visit(from, to));
        }
    }

    return NodeControl::Continue;
}
