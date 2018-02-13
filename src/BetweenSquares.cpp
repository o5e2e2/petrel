#include "BetweenSquares.hpp"

BetweenSquares::BetweenSquares () {
    FOR_INDEX(Square, from) {
        Bb belowFrom{ ::singleton<Bb::_t>(from)-1 };

        FOR_INDEX(Square, to) {
            Bb belowTo{ ::singleton<Bb::_t>(to)-1 };

            Bb betweenArea = (belowFrom ^ belowTo) % to;

            if      (from.horizont()[to]) { between[from][to] = betweenArea & from.horizont(); }
            else if (from.vertical()[to]) { between[from][to] = betweenArea & from.vertical(); }
            else if (from.diagonal()[to]) { between[from][to] = betweenArea & from.diagonal(); }
            else if (from.antidiag()[to]) { between[from][to] = betweenArea & from.antidiag(); }
            else                          { between[from][to] = Bb{}; }
        }
    }
}
