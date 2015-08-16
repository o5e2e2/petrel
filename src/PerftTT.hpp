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
                    //update the age of transpositioned entry
                    popup(i);
                }

                return b[i].getNodes();
            }
        }
        return 0;
    }

    void popup(Index i) {
        //move the i entry to the actual age region, reordering the rest

        ++counter.used;
        b[i].perft = perft(b[i].perft, counter.age);

        auto n = b[i].getNodes();

        for (Index j = 3; j > i; --j) {
            //seek the new slot for i
            if (!b[j].isOk() || b[j].getNodes() <= n) {
                popup(i, j);
                return;
            }
        }

        origin.save(i, m[i]);
        return;
    }

    void popup(Index i, Index j) {
        for (Index k = i; k < j; ++k) {
            origin.save(k, m[k+1]);
        }
        origin.save(j, m[i]);
    }

    void _out() {
        FOR_INDEX(Index, k) {
            if (!b[k].isOk()) { std::cout << '\''; }
            std::cout << b[k].getNodes() << ' ';
        }
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
        /*
        //replace the same entry
        FOR_INDEX(Index, i) {
            if (b[i].key == key(z, d)) {
                b[i].perft = perft(n, counter.age);
                popup(i);
                return;
            }
        }
        */

        Index i = 0;
        if (b[0].isOk()) {
            if (b[1].getNodes() <= n) {
                i = 1;
                if (b[2].getNodes() <= n) {
                    i = 2;
                    if (b[3].getNodes() <= n) {
                        origin.save(2, m[3]);
                        i = 3;
                    }
                }
            }
        }
        else if (b[1].isOk()) {
            if (b[1].getNodes() <= n) {
                origin.save(0, m[1]);
                i = 1;
                if (b[2].getNodes() <= n) {
                    origin.save(1, m[2]);
                    i = 2;
                    if (b[3].getNodes() <= n) {
                        origin.save(2, m[3]);
                        i = 3;
                    }
                }
            }
            ++counter.used;
        }
        else if (b[2].isOk()) {
            //shallowest of 2
            if (b[0].getNodes() < b[1].getNodes()) {
                origin.save(0, m[1]);
            }

            i = 1;
            if (b[2].getNodes() <= n) {
                origin.save(1, m[2]);
                i = 2;
                if (b[3].getNodes() <= n) {
                    origin.save(2, m[3]);
                    i = 3;
                }
            }
            ++counter.used;
        }
        else if (b[3].isOk()) {
            //shallowest of 3
            if (b[0].getNodes() < b[1].getNodes()) {
                if (b[0].getNodes() < b[2].getNodes()) {
                    origin.save(0, m[2]);
                }
            }
            else {
                if (b[1].getNodes() < b[2].getNodes()) {
                    origin.save(1, m[2]);
                }
            }

            i = 2;
            if (b[3].getNodes() <= n) {
                origin.save(2, m[3]);
                i = 3;
            }
            ++counter.used;
        }
        else {
            //shallowest of 4
            if (b[0].getNodes() < b[1].getNodes()) {
                if (b[2].getNodes() < b[3].getNodes()) {
                    if (b[0].getNodes() < b[2].getNodes()) {
                        origin.save(0, m[3]);
                    }
                    else {
                        origin.save(2, m[3]);
                    }
                }
                else {
                    if (b[0].getNodes() < b[3].getNodes()) {
                        origin.save(0, m[3]);
                    }
                }
            }
            else {
                if (b[2].getNodes() < b[3].getNodes()) {
                    if (b[1].getNodes() < b[2].getNodes()) {
                        origin.save(1, m[3]);
                    }
                    else {
                        origin.save(2, m[3]);
                    }
                }
                else {
                    if (b[1].getNodes() < b[3].getNodes()) {
                        origin.save(1, m[3]);
                    }
                }
            }

            i = 3;
            ++counter.used;
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
        //there are 7 ages, not 8, because of:
        //1) need to break 4*n ply transposition pattern
        //2) make sure that initally clear entry is never hidden
        auto a = (counter.age + 1) & 7;
        counter = {};
        counter.age = a? a : 1;
    }

    static node_count_t getUsed()  { return counter.used; }
    static node_count_t getHit()   { return counter.hit; }
    static node_count_t getTried() { return counter.tried; }

};

#endif
