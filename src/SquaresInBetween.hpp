#ifndef SQUARES_IN_BETWEEN_HPP
#define SQUARES_IN_BETWEEN_HPP

#include "Bb.hpp"

//possible queen moves on the empty board between two given squares (32k) (excluding both ends)
class SquaresInBetween {
    Square::arrayOf< Square::arrayOf<Bb> > inBetween;

public:
    constexpr SquaresInBetween () {
        FOR_INDEX(Square, from) {
            Bb belowFrom{ ::singleton<Bb::_t>(from)-1 };

            FOR_INDEX(Square, to) {
                Bb belowTo{ ::singleton<Bb::_t>(to)-1 };

                Bb areaInBetween = (belowFrom ^ belowTo) % to;

                if      (from.horizont().has(to)) { inBetween[from][to] = areaInBetween & from.horizont(); }
                else if (from.vertical().has(to)) { inBetween[from][to] = areaInBetween & from.vertical(); }
                else if (from.diagonal().has(to)) { inBetween[from][to] = areaInBetween & from.diagonal(); }
                else if (from.antidiag().has(to)) { inBetween[from][to] = areaInBetween & from.antidiag(); }
                else                              { inBetween[from][to] = Bb{}; }
            }
        }
    }

    constexpr const Bb& operator() (Square from, Square to) const { return inBetween[from][to]; }

};

extern const SquaresInBetween inBetween;

#endif
