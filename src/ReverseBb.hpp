#ifndef REVERSE_BB_HPP
#define REVERSE_BB_HPP

#include "BitArray128.hpp"
#include "BitArray128Reverse.hpp"
#include "Bb.hpp"

constexpr inline __m128i combine(Bb lo, Bb hi) {
    return __m128i{static_cast<__int64>(lo), static_cast<__int64>(hi)};
}

struct SingletonBb : Square::static_arrayOf<__m128i> {
    constexpr SingletonBb () {
        FOR_INDEX(Square, sq) {
            Square reversedSq(~File(sq), ~Rank(sq));
            (*this)[sq] = ::combine(Bb{sq}, Bb{reversedSq});
        }
    }
};
extern const SingletonBb singletonBb;

enum direction_t { Horizont, Vertical, Diagonal, Antidiag };
typedef Index<4, direction_t>::static_arrayOf<__m128i> Directions;
struct DirectionBb : SliderType::static_arrayOf< Square::static_arrayOf<Directions> > {
    constexpr DirectionBb () {
        const auto empty = ::combine(Bb{}, Bb{});
        FOR_INDEX(Square, sq) {
            Square reversedSq{~File(sq), ~Rank(sq)};

            (*this)[Queen][sq][Horizont] = ::combine(sq.horizont(), reversedSq.horizont());
            (*this)[Queen][sq][Vertical] = ::combine(sq.vertical(), reversedSq.vertical());
            (*this)[Queen][sq][Diagonal] = ::combine(sq.diagonal(), reversedSq.diagonal());
            (*this)[Queen][sq][Antidiag] = ::combine(sq.antidiag(), reversedSq.antidiag());

            (*this)[Rook][sq][Horizont] = ::combine(sq.horizont(), reversedSq.horizont());
            (*this)[Rook][sq][Vertical] = ::combine(sq.vertical(), reversedSq.vertical());
            (*this)[Rook][sq][Diagonal] = empty;
            (*this)[Rook][sq][Antidiag] = empty;

            (*this)[Bishop][sq][Horizont] = empty;
            (*this)[Bishop][sq][Vertical] = empty;
            (*this)[Bishop][sq][Diagonal] = ::combine(sq.diagonal(), reversedSq.diagonal());
            (*this)[Bishop][sq][Antidiag] = ::combine(sq.antidiag(), reversedSq.antidiag());
        }
    }
};
extern const DirectionBb directionBb;

/**
 * Vector of bitboard and its bitreversed complement
 * used for sliding pieces attacks generation
 */
class ReverseBb : public BitArray<ReverseBb, __m128i> {
    static _t hyperbola(_t v) { return v ^ ::bitReverse(v); }

public:
    explicit ReverseBb (Bb bb) : BitArray( hyperbola(_mm_cvtsi64_si128(static_cast<__int64>(bb))) ) {}

    Bb attack(SliderType type, Square from) const {
        const auto& dir = ::directionBb[type][from];
        const auto& fromBb = ::singletonBb[from];

        _t a = dir[Horizont] & _mm_sub_epi64(this->_v & dir[Horizont], fromBb);
        a ^=   dir[Vertical] & _mm_sub_epi64(this->_v & dir[Vertical], fromBb);
        a ^=   dir[Diagonal] & _mm_sub_epi64(this->_v & dir[Diagonal], fromBb);
        a ^=   dir[Antidiag] & _mm_sub_epi64(this->_v & dir[Antidiag], fromBb);

        return Bb{static_cast<Bb::_t>( _mm_cvtsi128_si64(hyperbola(a)) )};
    }

};

#endif
