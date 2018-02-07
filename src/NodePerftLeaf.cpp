#include "NodePerftLeaf.hpp"

bool NodePerftLeaf::visit(Square from, Square to) {
    CUT ( control.countNode() );

    playMove(parent, from, to, Zobrist{0});
    static_cast<NodePerft&>(parent).perft += getMoves().count();
    return false;
}
