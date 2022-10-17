#ifndef ATTACK_BB_HPP
#define ATTACK_BB_HPP

#include "BitArray128.hpp"
#include "BitReverse.hpp"
#include "Bb.hpp"

constexpr inline __m128i combine(Bb lo, Bb hi) {
    return __m128i{static_cast<__int64>(lo), static_cast<__int64>(hi)};
}

struct HyperbolaSq : Square::arrayOf<__m128i> {
    HyperbolaSq () {
        FOR_INDEX(Square, sq) {
            Square revSq(~File(sq), ~Rank(sq));
            (*this)[sq] = ::combine(Bb{sq}, Bb{revSq});
        }
    }
};
extern const HyperbolaSq hyperbolaSq;

enum direction_t { Horizont, Vertical, Diagonal, Antidiag };
typedef Index<4, direction_t>::arrayOf<__m128i> Directions;
struct HyperbolaDir : SliderType::arrayOf< Square::arrayOf<Directions> > {
    HyperbolaDir () {
        const auto empty = ::combine(Bb{}, Bb{});
        FOR_INDEX(Square, sq) {
            Square revSq{~File(sq), ~Rank(sq)};

            (*this)[Queen][sq][Horizont] = ::combine(sq.horizont(), revSq.horizont());
            (*this)[Queen][sq][Vertical] = ::combine(sq.vertical(), revSq.vertical());
            (*this)[Queen][sq][Diagonal] = ::combine(sq.diagonal(), revSq.diagonal());
            (*this)[Queen][sq][Antidiag] = ::combine(sq.antidiag(), revSq.antidiag());

            (*this)[Rook][sq][Horizont] = ::combine(sq.horizont(), revSq.horizont());
            (*this)[Rook][sq][Vertical] = ::combine(sq.vertical(), revSq.vertical());
            (*this)[Rook][sq][Diagonal] = empty;
            (*this)[Rook][sq][Antidiag] = empty;

            (*this)[Bishop][sq][Horizont] = empty;
            (*this)[Bishop][sq][Vertical] = empty;
            (*this)[Bishop][sq][Diagonal] = ::combine(sq.diagonal(), revSq.diagonal());
            (*this)[Bishop][sq][Antidiag] = ::combine(sq.antidiag(), revSq.antidiag());
        }
    }
};
extern const HyperbolaDir hyperbolaDir;

/**
 * Vector of bitboard and its bitreversed complement
 * used for sliding pieces attacks generation
 */
class AttackBb : public BitArray<AttackBb, __m128i> {
    typedef __m128i _t;
    _t occupied;

    static _t hyperbola(_t v) { return v ^ ::bitReverse(v); }

public:
    explicit AttackBb (Bb bb) : occupied( hyperbola(_mm_cvtsi64_si128(static_cast<__int64>(bb))) ) {}

    Bb attack(SliderType type, Square from) const {
        const auto& dir = ::hyperbolaDir[type][from];
        const auto& sq = ::hyperbolaSq[from];

        _t a;
        a  = dir[Horizont] & _mm_sub_epi64(occupied & dir[Horizont], sq);
        a ^= dir[Vertical] & _mm_sub_epi64(occupied & dir[Vertical], sq);
        a ^= dir[Diagonal] & _mm_sub_epi64(occupied & dir[Diagonal], sq);
        a ^= dir[Antidiag] & _mm_sub_epi64(occupied & dir[Antidiag], sq);
        a  = hyperbola(a);

        return Bb{static_cast<Bb::_t>( _mm_cvtsi128_si64(a) )};
    }

};

#endif
