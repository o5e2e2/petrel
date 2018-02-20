#include "NodePerftLeaf.hpp"
#include "NodePerft.hpp"
#include "SearchControl.hpp"

NodePerftLeaf::NodePerftLeaf(NodePerft& n) : Node(n) {}

bool NodePerftLeaf::visit(Square from, Square to) {
    CUT ( control.countNode() );

    playMove(parent, from, to, Zobrist{0});

    //updateParentPerft:
    static_cast<NodePerft&>(parent).perft += getMoves().count();
    return false;
}
