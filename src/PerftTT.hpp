#ifndef Perft_TT_HPP
#define Perft_TT_HPP

#include <limits>
#include "Zobrist.hpp"
#include "HashMemory.hpp"

class PerftTT {
    typedef unsigned age_t;
    typedef node_count_t used_t;
    typedef HashBucket::Index Index;

    static age_t age;
    static used_t used;

    enum {DepthShift = 6, AgeShift = 62};

    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xFF) << DepthShift;
    static const node_count_t AgeMask = static_cast<node_count_t>(3) << AgeShift;

    struct PerftRecord {
        Zobrist key;
        node_count_t perft;

        age_t getAge() {
            return static_cast<age_t>(static_cast<std::size_t>(perft) >> AgeShift);
        }

        depth_t getDepth() {
            return static_cast<depth_t>(static_cast<std::size_t>(key) >> DepthShift & 0xFF);
        }

        node_count_t getNodes() {
            return perft & ~AgeMask;
        }

    };
    typedef Index::array<PerftRecord> Bucket;

    union {
        HashBucket h;
        Bucket b;
    };

    static constexpr Zobrist key(Zobrist::_t z, depth_t d) {
        return Zobrist{ (z & ~DepthMask) | ((static_cast<decltype(z)>(static_cast<unsigned>(d)) << DepthShift) & DepthMask) };
    }

    static constexpr node_count_t perft(node_count_t n, age_t a) {
        return (n & ~AgeMask) | (static_cast<decltype(n)>(a) << AgeShift);
    }

    void set(Index i, Zobrist k, node_count_t n) {
        if (b[i].getAge() != age) {
            ++used;
        }
        b[i].key = k;
        b[i].perft = perft(n, age);
        h.save(i);
    }

public:
    PerftTT(HashBucket::_t* p) : h(p) {}

    node_count_t get(Zobrist z, depth_t d) {
        FOR_INDEX(Index, i) {
            if (b[i].key == key(z, d)) {
                if (b[i].getAge() != age) {
                    ++used;

                    b[i].perft = perft(b[i].perft, age);
                    h.save(i);
                }
                return b[i].getNodes();
            }
        }
        return 0;
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
        Zobrist k = key(z, d);

        Index min_i = 0;
        depth_t min_d = 0xFF;
        auto min_n = std::numeric_limits<node_count_t>::max();

        FOR_INDEX(Index, i) {
            if (b[i].key == k) { set(i, k, n); return; }

            depth_t i_d{ (b[i].getAge() == age)? b[i].getDepth() : 0 };
            auto i_n = b[i].getNodes();

            if (min_d >= i_d) {
                if (min_d == i_d && min_n <= i_n) {
                    continue;
                }
                min_i = i;
                min_d = i_d;
                min_n = i_n;
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
