#ifndef Perft_TT_HPP
#define Perft_TT_HPP

#include "Zobrist.hpp"
#include "HashMemory.hpp"

class PerftTT {
    typedef unsigned age_t;
    typedef node_count_t used_t;

    static age_t age;
    static used_t used;

    enum {DepthShift = 6, AgeShift = 62};

    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xFF) << DepthShift;
    static const node_count_t AgeMask = static_cast<node_count_t>(3) << AgeShift;

    struct PerftRecord {
        Zobrist key;
        node_count_t perft;
    };

    typedef PerftRecord Bucket[4];
    Bucket* record;

    static constexpr Zobrist key(Zobrist::_t z, depth_t d) {
        return Zobrist{ (z & ~DepthMask) | ((static_cast<decltype(z)>(static_cast<unsigned>(d)) << DepthShift) & DepthMask) };
    }

    static constexpr node_count_t perft(node_count_t n, age_t a) {
        return (n & ~AgeMask) | (static_cast<decltype(n)>(a) << AgeShift);
    }

    age_t getAge(index_t i) {
        return static_cast<age_t>(static_cast<std::size_t>((*record)[i].perft) >> AgeShift);
    }

    depth_t getDepth(index_t i) {
        return static_cast<depth_t>(static_cast<std::size_t>((*record)[i].key & DepthMask) >> DepthShift);
    }

    void set(index_t i, Zobrist k, node_count_t n) {
        if (getAge(i) != age) {
            ++used;
        }
        (*record)[i].key = k;
        (*record)[i].perft = perft(n, age);
    }

public:
    PerftTT(char* c) : record(reinterpret_cast<decltype(record)>(c)) {}

    node_count_t get(Zobrist z, depth_t d) {
        for (int i = 0; i < 4; ++i) {
            if ((*record)[i].key == key(z, d)) {
                if (getAge(i) != age) {
                    (*record)[i].perft = perft((*record)[i].perft, age);
                    ++used;
                }
                return (*record)[i].perft & ~AgeMask;
            }
        }
        return 0;
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
        Zobrist k = key(z, d);

        index_t min_i = 0;
        index_t min_d = 0xFF;

        for (index_t i = 0; i < 4; ++i) {
            Zobrist i_k = (*record)[i].key;
            if (i_k == k) { set(i, k, n); return; }

            auto i_a = getAge(i);

            index_t i_d = 0;
            if (i_a == age /*|| ((i_a + 1) & 3) == age*/) {
                i_d = getDepth(i);
            }

            if (min_d > i_d) {
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
