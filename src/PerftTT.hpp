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
        Zobrist::_t smallest_d = ~(static_cast<Zobrist::_t>(0));

        int best = 0;
        for (int i = 0; i < 4; ++i) {
            Zobrist::_t rk = (*record)[i].key;
            if (rk == k) { best = i; break; }

            if (smallest_d > (rk & DepthMask)) {
                smallest_d = (rk & DepthMask);
                best = i;
            }
        }

        (*record)[best].key = k;
        (*record)[best].perft = n;
    }
};

#endif
