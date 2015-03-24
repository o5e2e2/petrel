#ifndef Perft_TT_HPP
#define Perft_TT_HPP

#include "Zobrist.hpp"
#include "HashMemory.hpp"

class PerftTT {
    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xFF) << 6;

    struct PerftRecord {
        Zobrist::_t key;
        node_count_t perft;
    };

    typedef PerftRecord Bucket[4];
    Bucket* record;

    static Zobrist::_t key(Zobrist z, depth_t d) {
        return (static_cast<Zobrist::_t>(z) & ~DepthMask) | ((static_cast<Zobrist::_t>(d) << 6) & DepthMask);
    }

    void set(index_t i, Zobrist::_t k, node_count_t n) {
        (*record)[i].key = k;
        (*record)[i].perft = n;
    }

public:
    PerftTT(char* c) : record(reinterpret_cast<decltype(record)>(c)) {}

    node_count_t get(Zobrist z, depth_t d) const {
        for (int i = 0; i < 4; ++i) {
            if ((*record)[i].key == key(z, d)) { return (*record)[i].perft; }
        }
        return 0;
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
        Zobrist::_t k = key(z, d);

        Zobrist::_t rk = (*record)[0].key;
        if (rk == k) { set(0, k, n); return; }

        index_t min_i = 0;
        Zobrist::_t min_d = rk & DepthMask;

        for (index_t i = 1; i < 4; ++i) {
            rk = (*record)[i].key;
            if (rk == k) { set(i, k, n); return; }

            if ((rk & DepthMask) < min_d) {
                min_i = i;
                min_d = rk & DepthMask;
            }
        }

        set(min_i, k, n);
    }

};

#endif
