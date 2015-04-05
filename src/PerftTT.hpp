#ifndef Perft_TT_HPP
#define Perft_TT_HPP

#include "Zobrist.hpp"
#include "HashMemory.hpp"

class PerftTT {
    typedef index_t age_t;
    typedef node_count_t used_t;

    static index_t age;
    static used_t used;

    enum {DepthShift = 6, AgeShift = 62};

    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xFF) << DepthShift;
    static const node_count_t AgeMask = static_cast<node_count_t>(3) << AgeShift;

    struct PerftRecord {
        Zobrist::_t key;
        node_count_t perft;
    };

    typedef PerftRecord Bucket[4];
    Bucket* record;

    static Zobrist::_t key(Zobrist z, depth_t d) {
        return (static_cast<Zobrist::_t>(z) & ~DepthMask) | ((static_cast<Zobrist::_t>(d) << DepthShift) & DepthMask);
    }

    void set(index_t i, Zobrist::_t k, node_count_t n) {
        if (((*record)[i].perft >> AgeShift) != age) {
            ++used;
        }
        (*record)[i].key = k;
        (*record)[i].perft = (n & ~AgeMask) | (static_cast<node_count_t>(age) << AgeShift);
    }

public:
    PerftTT(char* c) : record(reinterpret_cast<decltype(record)>(c)) {}

    node_count_t get(Zobrist z, depth_t d) {
        for (int i = 0; i < 4; ++i) {
            if ((*record)[i].key == key(z, d)) {
                if (((*record)[i].perft >> AgeShift) != age) {
                    (*record)[i].perft = ((*record)[i].perft & ~AgeMask) | (static_cast<node_count_t>(age) << AgeShift);
                    ++used;
                }
                return (*record)[i].perft & ~AgeMask;
            }
        }
        return 0;
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
        Zobrist::_t k = key(z, d);

        index_t min_i = 0;
        index_t min_d = 0xFF;

        for (index_t i = 0; i < 4; ++i) {
            Zobrist::_t i_k = (*record)[i].key;
            if (i_k == k) { set(i, k, n); return; }

            index_t i_a = static_cast<index_t>((*record)[i].perft >> AgeShift);

            index_t i_d = 0;
            if (i_a == age || ((i_a + 1) & 3) == age) {
                i_d = static_cast<index_t>((i_k & DepthMask) >> DepthShift);
            }

            if (min_d >= i_d) {
                if (min_d == i_d && (*record)[i].perft > n) {
                    continue;
                }

                min_i = i;
                min_d = i_d;
            }
        }

        set(min_i, k, n);
    }

    static void resetAge() {
        age = 2;
        used = 0;
    }

    static void nextAge() {
        age = (age + 1) & 3;
        used = 0;
    }

    static node_count_t getUsed() {
        return used;
    }
};

#endif
