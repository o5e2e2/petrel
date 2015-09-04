#ifndef PERFT_TT_HPP
#define PERFT_TT_HPP

#include <limits>
#include "Zobrist.hpp"
#include "HashBucket.hpp"
#include "PerftRecord.hpp"
#include "SearchInfo.hpp"

class PerftTT {
    typedef HashBucket::Index Index;

    typedef Index::array<PerftRecord> Bucket;

    union {
        HashBucket h;
        Bucket b;
        __m128i m[4];
    };

    HashBucket& origin;

    void popup(Index i) {
        //move the i entry to the actual age region, reordering the rest
        b[i].updateAge();
        auto n = b[i].getNodes();

        for (Index j = 3; j > i; --j) {
            //seek the new slot for i
            if (!b[j].isOk() || b[j] <= n) {
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

public:
    PerftTT(HashBucket* p) : h(*p), origin(*p) {}

    node_count_t get(Zobrist z, depth_t d, SearchInfo& info) {
        ++info[TT_Tried];
        FOR_INDEX(Index, i) {
            if (b[i].isKeyMatch(z, d)) {
                ++info[TT_Hit];

                if (!b[i].isOk()) {
                    //update the age of transpositioned entry
                    ++info[TT_Used];
                    popup(i);
                }

                return b[i].getNodes();
            }
        }
        return 0;
    }

    void set(Zobrist z, depth_t d, node_count_t n, SearchInfo& info) {
        /*
        //replace the same entry
        FOR_INDEX(Index, i) {
            if (b[i].isKeyMatch(z, d)) {
                b[i].setPerft(n);
                popup(i);
                return;
            }
        }
        */

        Index i = 0;
        if (b[0].isOk()) {
            if (b[1] <= n) {
                i = 1;
                if (b[2] <= n) {
                    i = 2;
                    if (b[3] <= n) {
                        origin.save(2, m[3]);
                        i = 3;
                    }
                }
            }
        }
        else {
            ++info[TT_Used];

            if (b[1].isOk()) {
                if (b[1] <= n) {
                    origin.save(0, m[1]);
                    i = 1;
                    if (b[2] <= n) {
                        origin.save(1, m[2]);
                        i = 2;
                        if (b[3] <= n) {
                            origin.save(2, m[3]);
                            i = 3;
                        }
                    }
                }
            }
            else if (b[2].isOk()) {
                //shallowest of 2
                if (b[0] < b[1]) {
                    origin.save(0, m[1]);
                }

                i = 1;
                if (b[2] <= n) {
                    origin.save(1, m[2]);
                    i = 2;
                    if (b[3] <= n) {
                        origin.save(2, m[3]);
                        i = 3;
                    }
                }
            }
            else if (b[3].isOk()) {
                //shallowest of 3
                if (b[0] < b[1]) {
                    if (b[0] < b[2]) {
                        origin.save(0, m[2]);
                    }
                }
                else {
                    if (b[1] < b[2]) {
                        origin.save(1, m[2]);
                    }
                }

                i = 2;
                if (b[3] <= n) {
                    origin.save(2, m[3]);
                    i = 3;
                }
            }
            else {
                //shallowest of 4
                if (b[0] < b[1]) {
                    if (b[2] < b[3]) {
                        if (b[0] < b[2]) {
                            origin.save(0, m[3]);
                        }
                        else {
                            origin.save(2, m[3]);
                        }
                    }
                    else {
                        if (b[0] < b[3]) {
                            origin.save(0, m[3]);
                        }
                    }
                }
                else {
                    if (b[2] < b[3]) {
                        if (b[1] < b[2]) {
                            origin.save(1, m[3]);
                        }
                        else {
                            origin.save(2, m[3]);
                        }
                    }
                    else {
                        if (b[1] < b[3]) {
                            origin.save(1, m[3]);
                        }
                    }
                }

                i = 3;
            }
        }

        b[i].set(z, d, n);
        origin.save(i, m[i]);
    }

};

#endif
