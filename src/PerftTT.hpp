#ifndef PERFT_TT_HPP
#define PERFT_TT_HPP

#include <limits>
#include "Zobrist.hpp"
#include "HashBucket.hpp"
#include "PerftRecord.hpp"

class PerftTT {
    typedef HashBucket::Index Index;

    typedef Index::array<PerftRecord> Bucket;

    union {
        HashBucket h;
        Bucket b;
        __m128i m[4];
    };

    HashBucket& origin;

public:
    PerftTT(HashBucket* p) : h(*p), origin(*p) {}

    node_count_t get(Zobrist z, depth_t d) {
        HashBucket::tickTried();
        FOR_INDEX(Index, i) {
            if (b[i].isKeyMatch(z, d)) {
                HashBucket::tickHit();

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

        HashBucket::tickUsed();
        b[i].updateAge();

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
            std::cout << b[k] << ' ';
        }
        std::cout << '\n';
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
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
        else {
            HashBucket::tickUsed();

            if (b[1].isOk()) {
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
            }
        }

        b[i].set(z, d, n);
        origin.save(i, m[i]);
    }

};

#endif
