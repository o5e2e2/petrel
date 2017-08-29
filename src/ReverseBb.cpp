#include "ReverseBb.hpp"
#include "combineBb.hpp"

ReverseBb::Singleton::Singleton () {
    FOR_INDEX(Square, sq) {
        Square reversedSq(~File{sq}, ~Rank{sq});

        (*this)[sq] = ::combine(Bb{sq}, Bb{reversedSq});
    }
}

ReverseBb::Direction::Direction () {
    FOR_INDEX(Square, sq) {
        Square reversedSq{~File(sq), ~Rank(sq)};

        (*this)[Queen][sq][Horizont] = ::combine(Bb::horizont(sq), Bb::horizont(reversedSq));
        (*this)[Queen][sq][Vertical] = ::combine(Bb::vertical(sq), Bb::vertical(reversedSq));
        (*this)[Queen][sq][Diagonal] = ::combine(Bb::diagonal(sq), Bb::diagonal(reversedSq));
        (*this)[Queen][sq][Antidiag] = ::combine(Bb::antidiag(sq), Bb::antidiag(reversedSq));

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
