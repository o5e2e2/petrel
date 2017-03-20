#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "typedefs.hpp"

class SearchControl;
class PositionMoves;
class SearchWindow;

typedef bool SearchFn(const PositionMoves&, SearchWindow&);

namespace Perft {
    SearchFn perft;
    SearchFn divide;
    SearchFn perftRoot;
    SearchFn perftId;
}

#endif
