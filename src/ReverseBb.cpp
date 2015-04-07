#include "ReverseBb.hpp"
#include "DualBb.hpp"

ReverseBb::Singleton::Singleton () {
    FOR_INDEX(Square, sq) {
        Square r_sq(~File{sq}, ~Rank{sq});

        (*this)[sq] = ::combine(Bb{sq}, Bb{r_sq});
    }
}

ReverseBb::Direction::Direction () {
    FOR_INDEX(Square, sq) {
        Square r_sq{~File(sq), ~Rank(sq)};

        (*this)[Queen][sq][Horizont] = ::combine(Bb::horizont(sq), Bb::horizont(r_sq));
        (*this)[Queen][sq][Vertical] = ::combine(Bb::vertical(sq), Bb::vertical(r_sq));
        (*this)[Queen][sq][Diagonal] = ::combine(Bb::diagonal(sq), Bb::diagonal(r_sq));
        (*this)[Queen][sq][Antidiag] = ::combine(Bb::antidiag(sq), Bb::antidiag(r_sq));

        (*this)[Rook][sq][Horizont] = (*this)[Queen][sq][Horizont];
        (*this)[Rook][sq][Vertical] = (*this)[Queen][sq][Vertical];

        (*this)[Bishop][sq][Diagonal] = (*this)[Queen][sq][Diagonal];
        (*this)[Bishop][sq][Antidiag] = (*this)[Queen][sq][Antidiag];

        (*this)[Rook][sq][Diagonal] =
        (*this)[Rook][sq][Antidiag] =
        (*this)[Bishop][sq][Horizont] =
        (*this)[Bishop][sq][Vertical] = _mm_setzero_si128();
    }
}
