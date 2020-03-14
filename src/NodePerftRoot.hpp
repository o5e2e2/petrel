#ifndef NODE_PERFT_ROOT_HPP
#define NODE_PERFT_ROOT_HPP

#include "NodePerft.hpp"

class SearchLimit;
class SearchControl;

class NodePerftRoot : public NodePerft {
    bool isDivide;

    Control searchIteration();
    Control iterativeDeepening();

public:
    NodePerftRoot (const SearchLimit&, SearchControl&);
    Control visitChildren() override;
};

#endif
