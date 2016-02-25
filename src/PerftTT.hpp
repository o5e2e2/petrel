#ifndef PERFT_TT_HPP
#define PERFT_TT_HPP

#include "HashAge.hpp"
#include "HashBucket.hpp"
#include "PerftRecord.hpp"
#include "PerftRecordDual.hpp"
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

public:
    PerftTT(HashBucket* p, HashAge a) : m(*p), origin(*p), age(a) {}

    node_count_t get(Zobrist z, depth_t d);
    void set(Zobrist z, depth_t d, node_count_t n);

};

#endif
