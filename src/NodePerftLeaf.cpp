#include "NodePerftLeaf.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"

bool NodePerftLeaf::visit(Square from, Square to) {
    playMove(parent, from, to, Zobrist{0});
    control.info.decrementQuota();
    control.info.inc(PerftNodes, getMoves().count());
    return false;
}
