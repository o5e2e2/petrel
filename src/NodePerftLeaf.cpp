#include "NodePerftLeaf.hpp"
#include "NodePerft.hpp"
#include "SearchControl.hpp"

NodePerftLeaf::NodePerftLeaf(NodePerft& n) : Node(n) {}

bool NodePerftLeaf::visit(Square from, Square to) {
    playMove(parent, from, to, Zobrist{0});

    //updateParentPerft:
    auto& p = static_cast<NodePerft&>(parent);
    p.perft += movesCount;

    return false;
}
