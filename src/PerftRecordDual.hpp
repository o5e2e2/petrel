#ifndef PERFT_RECORD_DUAL_HPP
#define PERFT_RECORD_DUAL_HPP

#include "bitops128.hpp"
#include "PerftRecordSmall.hpp"

class PerftRecordDual {
public:
    typedef i128_t _t;
    typedef ::Index<2> Index;

    union {
        _t m;
        Index::arrayOf<PerftRecordSmall> s;
    };

public:
    constexpr const PerftRecordSmall& operator[] (Index i) const { return s[i]; }

    void set(Index i, Zobrist::_t z, node_count_t n) {
        s[i].set(z, n);
    }

};

#endif
