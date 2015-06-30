#include "BetweenSquares.hpp"

BetweenSquares::BetweenSquares () {
    FOR_INDEX(Square, from) {
        Bb belowFrom{ ::singleton<Bb::_t>(from)-1 };

        FOR_INDEX(Square, to) {
            Bb belowTo{ ::singleton<Bb::_t>(to)-1 };

            Bb betweenArea = (belowFrom ^ belowTo) % to;

            if      (Bb::horizont(from)[to]) { between[from][to] = betweenArea & Bb::horizont(from); }
            else if (Bb::vertical(from)[to]) { between[from][to] = betweenArea & Bb::vertical(from); }
            else if (Bb::diagonal(from)[to]) { between[from][to] = betweenArea & Bb::diagonal(from); }
            else if (Bb::antidiag(from)[to]) { between[from][to] = betweenArea & Bb::antidiag(from); }
            else                             { between[from][to] = Bb{}; }
        }
    }
}
