#ifndef NODE_PERFT_ROOT_HPP
#define NODE_PERFT_ROOT_HPP

#include "NodePerft.hpp"
#include "SearchLimit.hpp"
#include "SearchControl.hpp"

class NodePerftRoot : public NodePerft {
public:
    NodePerftRoot (const PositionMoves& p, SearchControl& c, ply_t d): NodePerft(p, c, d) {}
    NodeControl visitChildren() override;
};

class NodePerftRootDepth : public NodePerftRoot {
public:
    NodePerftRootDepth (const SearchLimit& l, SearchControl& c): NodePerftRoot(l.positionMoves, c, l.depth) {}
    NodeControl visitChildren() override;
};

class NodePerftRootIterative : public NodePerftRoot {
public:
    NodePerftRootIterative (const SearchLimit& l, SearchControl& c): NodePerftRoot(l.positionMoves, c, l.depth) {}
    NodeControl visitChildren() override;
};

class NodePerftDivide : public NodePerftRoot {
    index_t moveCount = 0;
public:
    NodePerftDivide (const PositionMoves& p, SearchControl& c, ply_t d): NodePerftRoot(p, c, d) {}
    NodeControl visitChildren() override;
    NodeControl visit(Square, Square) override;
};

class NodePerftDivideDepth : public NodePerftDivide {
public:
    NodePerftDivideDepth (const SearchLimit& l, SearchControl& c): NodePerftDivide(l.positionMoves, c, l.depth) {}
    NodeControl visitChildren() override;
};

class NodePerftDivideIterative : public NodePerftDivide {
public:
    NodePerftDivideIterative (const SearchLimit& l, SearchControl& c): NodePerftDivide(l.positionMoves, c, l.depth) {}
    NodeControl visitChildren() override;
};

#endif
