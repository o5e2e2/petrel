#ifndef BETWEEN_SQUARES_HPP
#define BETWEEN_SQUARES_HPP

#include "Bb.hpp"

//queen moves between two given squares (32k) (excluding both ends)
class BetweenSquares {
    Square::static_array< Square::static_array<Bb> > between;

public:
    constexpr BetweenSquares () {
        FOR_INDEX(Square, from) {
            Bb belowFrom{ ::singleton<Bb::_t>(from)-1 };

            FOR_INDEX(Square, to) {
                Bb belowTo{ ::singleton<Bb::_t>(to)-1 };

                Bb betweenArea = (belowFrom ^ belowTo) % to;

                if      (from.horizont().has(to)) { between[from][to] = betweenArea & from.horizont(); }
                else if (from.vertical().has(to)) { between[from][to] = betweenArea & from.vertical(); }
                else if (from.diagonal().has(to)) { between[from][to] = betweenArea & from.diagonal(); }
                else if (from.antidiag().has(to)) { between[from][to] = betweenArea & from.antidiag(); }
                else                              { between[from][to] = Bb{}; }
            }
        }
    }

    constexpr const Bb& operator() (Square from, Square to) const {
        return between[from][to];
    }

};

extern const BetweenSquares between;

#endif
