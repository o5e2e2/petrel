#ifndef BETWEEN_SQUARES_HPP
#define BETWEEN_SQUARES_HPP

#include "Bb.hpp"

//queen moves between two given squares (32k) (excluding both ends)
class BetweenSquares {
    Square::static_array< Square::static_array<Bb> > between;

public:
    BetweenSquares ();

    const Bb& operator() (Square from, Square to) const {
        return between[from][to];
    }

};

extern const BetweenSquares between;

#endif
