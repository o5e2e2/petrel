#include "ReverseBb.hpp"
#include "combineBb.hpp"

ReverseBb::Singleton::Singleton () {
    FOR_INDEX(Square, sq) {
        Square reversedSq(~File(sq), ~Rank(sq));

        (*this)[sq] = ::combine(Bb{sq}, Bb{reversedSq});
    }
}

ReverseBb::Direction::Direction () {
    FOR_INDEX(Square, sq) {
        Square reversedSq{~File(sq), ~Rank(sq)};

        (*this)[Queen][sq][Horizont] = ::combine(sq.horizont(), reversedSq.horizont());
        (*this)[Queen][sq][Vertical] = ::combine(sq.vertical(), reversedSq.vertical());
        (*this)[Queen][sq][Diagonal] = ::combine(sq.diagonal(), reversedSq.diagonal());
        (*this)[Queen][sq][Antidiag] = ::combine(sq.antidiag(), reversedSq.antidiag());

        (*this)[Rook][sq][Horizont] = ::combine(sq.horizont(), reversedSq.horizont());
        (*this)[Rook][sq][Vertical] = ::combine(sq.vertical(), reversedSq.vertical());

        (*this)[Bishop][sq][Diagonal] = ::combine(sq.diagonal(), reversedSq.diagonal());
        (*this)[Bishop][sq][Antidiag] = ::combine(sq.antidiag(), reversedSq.antidiag());

        (*this)[Rook][sq][Diagonal] =
        (*this)[Rook][sq][Antidiag] =
        (*this)[Bishop][sq][Horizont] =
        (*this)[Bishop][sq][Vertical] = _mm_setzero_si128();
    }
}
