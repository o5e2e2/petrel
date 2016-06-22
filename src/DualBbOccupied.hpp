#ifndef DUAL_BB_OCCUPIED_HPP
#define DUAL_BB_OCCUPIED_HPP

#include "typedefs.hpp"
#include "combineBb.hpp"
#include "BitArray128Reverse.hpp"

class DualBbOccupied {
    typedef __m128i _t;

    union {
        _t _v;
        Side::array<Bb> occupied;
    };

public:
    constexpr DualBbOccupied () : _v{0, 0} {}
    constexpr const Bb& operator [] (Side si) const { return occupied[si]; }

    DualBbOccupied (Bb my, Bb op) {
        _v = ::combine(my, op);
        _v += ::bitReverse.byte_swap(_v);
    }

    DualBbOccupied& operator= (DualBbOccupied&& d) { _v = d._v; return *this; }

    void swap() { _v = _mm_swap_epi64(_v); }

};

#endif
