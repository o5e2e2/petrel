#include "BetweenSquares.hpp"

BetweenSquares::BetweenSquares () {
    FOR_INDEX(Square, from) {
        Bb bb_from{::singleton<Bb::_t>(from)-1};
        FOR_INDEX(Square, to) {
            Bb bb_to{::singleton<Bb::_t>(to)-1};
            Bb bb{(bb_from ^ bb_to) % to};

            if (Bb::horizont(from)[to]) {
                between[from][to] = bb & Bb::horizont(from);
            }
            else if (Bb::vertical(from)[to]) {
                between[from][to] = bb & Bb::vertical(from);
            }
            else if (Bb::diagonal(from)[to]) {
                between[from][to] = bb & Bb::diagonal(from);
            }
            else if (Bb::antidiag(from)[to]) {
                between[from][to] = bb & Bb::antidiag(from);
            }
            else {
                between[from][to] = Bb::empty();
            }
        }
    }
}
