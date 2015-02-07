#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "typedefs.hpp"

class SearchControl;
class Position;

typedef bool SearchFn(SearchControl&, const Position&, depth_t);

namespace Perft {
    SearchFn perft;
}

namespace PerftDivide {
    SearchFn perft;
}

namespace PerftRoot {
    SearchFn perft;
}

namespace PerftDivideRoot {
    SearchFn perft;
}

#endif
