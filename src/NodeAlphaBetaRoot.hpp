#ifndef NODE_ALPHA_BETA_ROOT_HPP
#define NODE_ALPHA_BETA_ROOT_HPP

#include "NodeAlphaBeta.hpp"

class SearchLimit;

class NodeAlphaBetaRoot : public NodeAlphaBeta {

    NodeControl searchIteration();
    NodeControl iterativeDeepening();

public:
    NodeAlphaBetaRoot (const SearchLimit&, SearchControl&);
    NodeControl visitChildren() override;
};

#endif
