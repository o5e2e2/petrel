#include "BetweenSquares.hpp"

BetweenSquares::BetweenSquares () {
    FOR_INDEX(Square, from) {
        Bb b_from{::singleton<Bb::_t>(from)-1};
        FOR_INDEX(Square, to) {
            Bb b_to{::singleton<Bb::_t>(to)-1};
            Bb b{(b_from ^ b_to) % to};

            if (Bb::horizont(from)[to]) {
                between[from][to] = b & Bb::horizont(from);
            }
            else if (Bb::vertical(from)[to]) {
                between[from][to] = b & Bb::vertical(from);
            }
            else if (Bb::diagonal(from)[to]) {
                between[from][to] = b & Bb::diagonal(from);
            }
            else if (Bb::antidiag(from)[to]) {
                between[from][to] = b & Bb::antidiag(from);
            }
            else {
                between[from][to] = Bb::empty();
            }
        }
    }
}
