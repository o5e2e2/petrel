#ifndef PERFT_RECORD_DUAL_HPP
#define PERFT_RECORD_DUAL_HPP

#include "bitops128.hpp"
#include "PerftRecordSmall.hpp"

class PerftRecordDual {
public:
    typedef ::Index<2> Index;

    union {
        __m128i m;
        Index::arrayOf<PerftRecordSmall> s;
    };

public:
    constexpr const PerftRecordSmall& operator[] (Index i) const { return s[i]; }

    void set(Index i, Zobrist::_t z, node_count_t n) {
        s[i].set(z, n);
    }

};

#endif
