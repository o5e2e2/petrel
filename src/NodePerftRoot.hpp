#ifndef NODE_PERFT_ROOT_HPP
#define NODE_PERFT_ROOT_HPP

#include "NodePerft.hpp"

class SearchLimit;
class SearchControl;

class NodePerftRoot : public NodePerft {
    bool isDivide;

    bool searchIteration();
    bool iterativeDeepening();

public:
    NodePerftRoot (const SearchLimit&, SearchControl&);
    bool visitChildren() override;
};

#endif
