#ifndef Perft_TT_HPP
#define Perft_TT_HPP

#include <limits>
#include "Zobrist.hpp"
#include "HashMemory.hpp"

class PerftTT {
    typedef unsigned age_t;
    typedef node_count_t used_t;
    typedef HashBucket::Index Index;

    struct Counter {
        used_t tried;
        used_t hit;
        used_t used;
        age_t age;

        constexpr Counter () : tried{0}, hit{0}, used{0}, age{0} {}
    };

    static Counter counter;


    enum {DepthShift = 6, AgeShift = 61};

    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xFF) << DepthShift;
    static const node_count_t AgeMask = static_cast<node_count_t>(7) << AgeShift;

    struct PerftRecord {
        Zobrist key;
        node_count_t perft;

        age_t getAge() {
            return static_cast<age_t>(static_cast<std::size_t>(perft) >> AgeShift);
        }

        bool isOk() {
            return getAge() == counter.age;
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
        __m128i m[4];
    };

    HashBucket& origin;

    static constexpr Zobrist key(Zobrist::_t z, depth_t d) {
        return Zobrist{ (z & ~DepthMask) | ((static_cast<decltype(z)>(static_cast<unsigned>(d)) << DepthShift) & DepthMask) };
    }

    static constexpr node_count_t perft(node_count_t n, age_t a) {
        return (n & ~AgeMask) | (static_cast<decltype(n)>(a) << AgeShift);
    }

public:
    PerftTT(HashBucket* p) : h(*p), origin(*p) {}

    node_count_t get(Zobrist z, depth_t d) {
        ++counter.tried;
        FOR_INDEX(Index, i) {
            if (b[i].key == key(z, d)) {
                ++counter.hit;
                if (!b[i].isOk()) {
                    popup(i);
                }

                return b[i].getNodes();
            }
        }
        return 0;
    }

    void popup(Index i) {
        ++counter.used;
        b[i].perft = perft(b[i].perft, counter.age);

        auto n = b[i].getNodes();

        for (Index j = 3; j > i; --j) {
            //seek the new slot for i
            if (! (b[i].isOk() && n < b[i].getNodes()) ) {
                //popup i to j
                for (Index k = i; k < j; ++k) {
                    origin.save(k, m[k+1]);
                }
                origin.save(j, m[i]);
                return;
            }
        }

        origin.save(i, m[i]);
        return;
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
        /*
        FOR_INDEX(Index, i) {
            if (b[i].key == key(z, d)) {
                if (!b[i].isOk()) {
                    popup(i);
                }
                return;
            }
        }
        */

        Index i;
        if (!b[0].isOk()) {
            ++counter.used;

            if      (b[1].isOk() && n < b[1].getNodes()) {
                i = 0;
            }
            else if (b[2].isOk() && n < b[2].getNodes()) {
                i = 1;
                origin.save(0, m[1]);
            }
            else if (b[3].isOk() && n < b[3].getNodes()) {
                i = 2;
                origin.save(0, m[1]);
                origin.save(1, m[2]);
            }
            else {
                i = 3;
                origin.save(0, m[1]);
                origin.save(1, m[2]);
                origin.save(2, m[3]);
            }
        }
        else {
            if (n < b[2].getNodes()) {
                if (n < b[1].getNodes()) {
                    i = 0;
                }
                else {
                    i = 1;
                }
            }
            else {
                if (n < b[3].getNodes()) {
                    i = 2;
                }
                else {
                    i = 3;
                    origin.save(2, m[3]);
                }
            }
        }

        b[i].key = key(z, d);
        b[i].perft = perft(n, counter.age);
        origin.save(i, m[i]);
    }

    static void clearAge() {
        counter = {};
        counter.age = 1;
    }

    static void nextAge() {
        auto a = (counter.age + 1) & 7;
        counter = {};
        counter.age = a? a : 1;
    }

    static node_count_t getUsed()  { return counter.used; }
    static node_count_t getHit()   { return counter.hit; }
    static node_count_t getTried() { return counter.tried; }

};

#endif
