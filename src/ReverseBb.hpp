#ifndef REVERSE_BB_HPP
#define REVERSE_BB_HPP

#include "Vector.hpp"
#include "BitArray.hpp"
#include "Bb.hpp"

#if !defined PLATFORM_64
    inline __int64 _mm_cvtsi128_si64(__m128i v) {
        auto lo = _mm_cvtsi128_si32(v);
        auto hi = _mm_cvtsi128_si32(_mm_shuffle_epi32(v, _MM_SHUFFLE(1, 1, 1, 1)));
        return ::combine(lo, hi);
    }

    inline __m128i _mm_cvtsi64_si128(__int64 b) {
        return _mm_unpacklo_epi32(_mm_cvtsi32_si128(::lo(b)), _mm_cvtsi32_si128(::hi(b)));
    }
#endif

/**
 * Vector of bitboard and its bitreversed complement
 * used for sliding pieces attacks generation
 */
class ReverseBb : public BitArray<ReverseBb, __m128i> {
    typedef BitArray<ReverseBb, __m128i> Base;

    enum direction_t { Horizont, Vertical, Diagonal, Antidiag };
    typedef Index<4, direction_t>::static_array<_t> Directions;

    struct Direction : SliderType::static_array< Square::static_array<Directions> > { Direction (); };
    static const Direction direction;

    struct Singleton : Square::static_array<_t> { Singleton (); };
    static const Singleton singleton;

    class BitReverse {
        typedef __m128i _t;
        _t lo_nibble_reverse, hi_nibble_reverse, lo_nibble_mask, byte_reverse;
    public:
        BitReverse ();

        _t operator() (_t v) const {
            _t lo = _mm_shuffle_epi8(lo_nibble_reverse, lo_nibble_mask & v);
            _t hi = _mm_shuffle_epi8(hi_nibble_reverse, lo_nibble_mask & _mm_srli_epi16(v, 4));
            return _mm_shuffle_epi8(lo ^ hi, byte_reverse);
        }
    };
    static const BitReverse bit_reverse;

    static _t hyperbola(_t v) { return v ^ bit_reverse(v); }
    static _t combine(Bb, Bb);

    explicit operator Bb () const { return Bb(static_cast<Bb::_t>(_mm_cvtsi128_si64(static_cast<_t>(*this)))); }

public:
    explicit ReverseBb (Bb b) : Base( hyperbola(_mm_cvtsi64_si128(static_cast<__int64>(b))) ) {}

    Bb attack(SliderType type, Square from) const {
        const Directions& dir = direction[type][from];

        _t a = dir[Horizont] & _mm_sub_epi64(static_cast<_t>(*this) & dir[Horizont], singleton[from]);
        a ^=   dir[Vertical] & _mm_sub_epi64(static_cast<_t>(*this) & dir[Vertical], singleton[from]);
        a ^=   dir[Diagonal] & _mm_sub_epi64(static_cast<_t>(*this) & dir[Diagonal], singleton[from]);
        a ^=   dir[Antidiag] & _mm_sub_epi64(static_cast<_t>(*this) & dir[Antidiag], singleton[from]);

        return Bb{static_cast<Bb::_t>( _mm_cvtsi128_si64(hyperbola(a)) )};
    }

};

#endif
