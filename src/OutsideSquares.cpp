#include "OutsideSquares.hpp"

OutsideSquares::OutsideSquares () {
    FOR_INDEX(Square, from) {
        Bb belowFrom{ ::singleton<Bb::_t>(from)-1 };

        FOR_INDEX(Square, to) {
            Bb belowTo{ ::singleton<Bb::_t>(to)-1 };
            Bb outsideArea = (from >= to)? belowTo : Bb{~static_cast<Bb::_t>(belowTo)} - to;

            if      (Bb::horizont(from)[to]) { outside[from][to] = outsideArea & Bb::horizont(from); }
            else if (Bb::vertical(from)[to]) { outside[from][to] = outsideArea & Bb::vertical(from); }
            else if (Bb::diagonal(from)[to]) { outside[from][to] = outsideArea & Bb::diagonal(from); }
            else if (Bb::antidiag(from)[to]) { outside[from][to] = outsideArea & Bb::antidiag(from); }
            else                             { outside[from][to] = Bb{}; }
        }
    }
}
