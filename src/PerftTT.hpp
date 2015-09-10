#ifndef PERFT_TT_HPP
#define PERFT_TT_HPP

#include <limits>
#include "HashAge.hpp"
#include "HashBucket.hpp"
#include "PerftRecord.hpp"
#include "PerftRecordDual.hpp"
#include "SearchInfo.hpp"
#include "Zobrist.hpp"

class PerftTT {
    typedef HashBucket::Index Index;

    typedef ::Index<3>::array<PerftRecord> Bucket;

    union {
        HashBucket m;
        struct {
            Bucket b;
            PerftRecordDual s;
        } b;
    };

    HashBucket& origin;
    HashAge age;

    void backup(Index i) {
        if (b.b[i].getDepth() == 1) {
            b.s.set(b.b[i].getDepth(), b.b[i].getZobrist());
            origin.save(3, m[3]);
        }
    }

    void popup(::Index<3> i) {
        //move the i entry to the actual age region, reordering the rest
        b.b[i].updateAge(age);
        auto n = b.b[i].getNodes();

        for (::Index<3> j = ::Index<3>::Size-1; j > i; --j) {
            //seek the new slot for i
            if (!b.b[j].isOk(age) || b.b[j] <= n || b.b[j].getDepth() <= 1) {
                for (auto k = i; k < j; ++k) {
                    origin.save(+k, m[k+1]);
                }
                origin.save(+j, m[+i]);
                return;
            }
        }

        origin.save(+i, m[+i]);
        return;
    }

public:
    PerftTT(HashBucket* p, HashAge a) : m(*p), origin(*p), age(a) {}

    node_count_t get(Zobrist z, depth_t d, SearchInfo& info) {
        ++info[TT_Tried];

        FOR_INDEX(::Index<3>, i) {
            if (b.b[i].isKeyMatch(z, d)) {
                ++info[TT_Hit];

                if (!b.b[i].isOk(age)) {
                    //update the age of transpositioned entry
                    ++info[TT_Used];
                    popup(i);
                }

                return b.b[i].getNodes();
            }
        }

        if (d == 1) {
            FOR_INDEX(PerftRecordDual::Index, i) {
                if (b.s[i].isKeyMatch(z)) {
                    ++info[TT_Hit];

                    if (i == 1) {
                        b.s.swap();
                        origin.save(3, m[3]);
                    }

                    return b.s[0].getNodes();
                }
            }
        }

        return 0;
    }

    void set(Zobrist z, depth_t d, node_count_t n, SearchInfo& info) {
        /*
        //replace the same entry
        for (Index i = 1; i.isOk(); ++i) {
            if (b.b[i].isKeyMatch(z, d)) {
                b.b[i].setPerft(n);
                popup(i);
                return;
            }
        }

        if (d == 1) {
            FOR_INDEX(PerftRecordDual::Index, i) {
                if (b.s[i].isKeyMatch(z)) {
                    if (i == 1) { b.s[0].swap(); }
                    return;
                }
            }
        }
        */

        Index i = 0;
        if (b.b[0].isOk(age)) {
            if (d == 1 && b.b[0].getDepth() > 1) {
                b.s.set(z, n);
                origin.save(3, m[3]);
                return;
            }

            if (b.b[1] <= n) {
                backup(1);

                i = 1;
                if (b.b[2] <= n) {
                    origin.save(1, m[2]);
                    i = 2;
                }
            }
            else {
                backup(0);
            }
        }
        else {
            ++info[TT_Used];

            if (b.b[1].isOk(age)) {
                backup(0);
                if (b.b[1] <= n) {
                    origin.save(0, m[1]);

                    i = 1;
                    if (b.b[2] <= n) {
                        backup(1);
                        origin.save(1, m[2]);
                        i = 2;
                    }
                }
            }
            else if (b.b[2].isOk(age)) {
                //shallowest of 2
                if (b.b[0] < b.b[1]) {
                    backup(0);
                    origin.save(0, m[1]);
                }
                else {
                    backup(1);
                }

                i = 1;
                if (b.b[2] <= n) {
                    origin.save(1, m[2]);
                    i = 2;
                }
            }
            else {
                //shallowest of 3
                if (b.b[0] < b.b[1]) {
                    if (b.b[0] < b.b[2]) {
                        backup(0);
                        origin.save(0, m[2]);
                    }
                    else {
                        backup(2);
                    }
                }
                else {
                    if (b.b[1] < b.b[2]) {
                        backup(1);
                        origin.save(1, m[2]);
                    }
                    else {
                        backup(2);
                    }
                }

                i = 2;
            }
        }

        b.b[i].set(z, d, n, age);
        origin.save(i, m[i]);
    }

};

#endif
