#ifndef NODE_AB_ROOT_HPP
#define NODE_AB_ROOT_HPP

#include "NodeAbPv.hpp"

class SearchLimit;

class NodeAbRoot : public NodeAbPv {
    ply_t depthLimit;

    NodeControl searchIteration();
    NodeControl iterativeDeepening();

public:
    NodeAbRoot (const SearchLimit&, SearchControl&);
    NodeControl visitChildren() override;
};

#endif
