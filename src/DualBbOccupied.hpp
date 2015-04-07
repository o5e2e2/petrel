#ifndef DUAL_BB_OCCUPIED_HPP
#define DUAL_BB_OCCUPIED_HPP

#include "typedefs.hpp"
#include "DualBb.hpp"
#include "BitArray128Reverse.hpp"

class DualBbOccupied {
    typedef __m128i _t;

    union {
        _t _v;
        Side::array<Bb> occupied;
    };

public:
    constexpr DualBbOccupied () : _v{0, 0} {}
    constexpr const Bb& operator [] (Side side) const { return occupied[side]; }

    DualBbOccupied (Bb my, Bb op) {
        _v = ::combine(my, op);
        _v += bit_reverse.byte_swap(_v);
    }

    DualBbOccupied& operator= (DualBbOccupied&& d) { _v = d._v; return *this; }

    DualBbOccupied& swap() {
        _v = _mm_shuffle_epi32(_v, _MM_SHUFFLE(1, 0, 3, 2));
        return *this;
    }

};

#endif
