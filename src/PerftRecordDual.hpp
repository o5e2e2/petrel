#ifndef PERFT_RECORD_DUAL_HPP
#define PERFT_RECORD_DUAL_HPP

#include "bitops128.hpp"
#include "PerftRecordSmall.hpp"

class PerftRecordDual {
public:
    typedef ::Index<2> Index;

    union {
        __m128i m;
        Index::array<PerftRecordSmall> s;
    };

public:
    constexpr const PerftRecordSmall& operator[] (Index i) const { return s[i]; }

    void swap() {
        m = _mm_swap_epi64(m);
    }

    void set(Zobrist::_t z, node_count_t n) {
        swap();
        s[0].set(z, n);
    }

};

#endif
