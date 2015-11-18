#ifndef OUTSIDE_SQUARES_HPP
#define OUTSIDE_SQUARES_HPP

#include "Bb.hpp"

//queen moves ray from the given square outside the second given quare (32k) (excluding both points)
class OutsideSquares {
    Square::static_array< Square::static_array<Bb> > outside;

public:
    OutsideSquares ();

    const Bb& operator() (Square from, Square to) const {
        return outside[from][to];
    }

};

extern const OutsideSquares outside;

#endif
