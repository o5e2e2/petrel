#ifndef REVERSE_BB_HPP
#define REVERSE_BB_HPP

#include "BitArray128.hpp"
#include "BitArray128Reverse.hpp"
#include "Bb.hpp"

/**
 * Vector of bitboard and its bitreversed complement
 * used for sliding pieces attacks generation
 */
class ReverseBb : public BitArray<ReverseBb, __m128i> {
    enum direction_t { Horizont, Vertical, Diagonal, Antidiag };
    typedef Index<4, direction_t>::static_array<_t> Directions;

    struct Direction : SliderType::static_array< Square::static_array<Directions> > { Direction (); };
    static const Direction direction;

    struct Singleton : Square::static_array<_t> { Singleton (); };
    static const Singleton singleton;

    static _t hyperbola(_t v) { return v ^ ::bitReverse(v); }

    explicit operator Bb () const { return Bb{ static_cast<Bb::_t>( _mm_cvtsi128_si64(this->_v) ) }; }

public:
    explicit ReverseBb (Bb bb) : BitArray( hyperbola(_mm_cvtsi64_si128(static_cast<__int64>(bb))) ) {}

    Bb attack(SliderType type, Square from) const {
        const Directions& dir = direction[type][from];

        _t a = dir[Horizont] & _mm_sub_epi64(this->_v & dir[Horizont], singleton[from]);
        a ^=   dir[Vertical] & _mm_sub_epi64(this->_v & dir[Vertical], singleton[from]);
        a ^=   dir[Diagonal] & _mm_sub_epi64(this->_v & dir[Diagonal], singleton[from]);
        a ^=   dir[Antidiag] & _mm_sub_epi64(this->_v & dir[Antidiag], singleton[from]);

        return Bb{ static_cast<Bb::_t>( _mm_cvtsi128_si64(hyperbola(a)) ) };
    }

};

#endif
