#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "typedefs.hpp"

class SearchControl;
class Position;
class SearchWindow;

typedef bool SearchFn(const Position&, SearchWindow&);

namespace Perft {
    SearchFn perft;
}

namespace PerftDivide {
    SearchFn perft;
}

namespace PerftRoot {
    SearchFn perft;
}

#endif
