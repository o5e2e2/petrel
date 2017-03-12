#ifndef DUAL_BB_OCCUPIED_HPP
#define DUAL_BB_OCCUPIED_HPP

#include "typedefs.hpp"
#include "combineBb.hpp"
#include "BitArray128Reverse.hpp"

class DualBbOccupied {
    typedef __m128i _t;
    _t _v;

public:
    constexpr DualBbOccupied () : _v{0, 0} {};
    constexpr DualBbOccupied (const DualBbOccupied& o) = default;

    DualBbOccupied (Bb my, Bb op) {
        _v = ::combine(my, op);
        _v += ::bitReverse.byte_swap(_v);
    }

    constexpr const Bb& operator [] (Side si) const {
        return reinterpret_cast<const Side::array<Bb>&>(*this)[si];
    }

    void swap() { _v = _mm_swap_epi64(_v); }

};

#endif
