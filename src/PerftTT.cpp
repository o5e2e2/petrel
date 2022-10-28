#include "PerftTT.hpp"
#include "HashBucket.hpp"
#include "PerftRecord.hpp"
#include "PerftRecordSmall.hpp"

namespace {
    union BucketUnion {
        HashBucket m;
        struct {
            Index<3>::arrayOf<PerftRecord> b;
            Index<2>::arrayOf<PerftRecordSmall> d;
        } v;
    };
}

PerftTT::PerftTT () : hashMemory(sizeof(HashBucket)) {
}

node_count_t PerftTT::get(Zobrist z, ply_t d) {
    ++counter.reads;

    auto origin = reinterpret_cast<HashBucket*>(hashMemory.seek(z));
    BucketUnion u = *reinterpret_cast<BucketUnion*>(origin);

    if (d < 2 && u.v.d[d].isKeyMatch(z, d)) {
        ++counter.hits;
        return u.v.d[d].getNodes();
    }

    if (u.v.b[2].isKeyMatch(z, d)) {
        ++counter.hits;
        auto n = u.v.b[2].getNodes();
        if (u.v.b[1].getDepth() <= d) {
            u.v.b[2].setAge(hashAge);
            if (u.v.b[0].getDepth() <= d) {
                origin->set(0, u.m[2]);
                origin->set(1, u.m[0]);
            }
            else {
                origin->set(1, u.m[2]);
            }
            origin->set(2, u.m[1]);
        }
        return n;
    }

    if (u.v.b[1].isKeyMatch(z, d)) {
        ++counter.hits;
        auto n = u.v.b[1].getNodes();
        if (u.v.b[0].getDepth() <= d) {
            u.v.b[1].setAge(hashAge);
            origin->set(1, u.m[0]);
            origin->set(0, u.m[1]);
        }
        return n;
    }

    if (u.v.b[0].isKeyMatch(z, d)) {
        ++counter.hits;
        auto n = u.v.b[0].getNodes();
        return n;
    }

    return NodeCountNone;
}

void PerftTT::set(Zobrist z, ply_t d, node_count_t n) {
    ++counter.writes;

    auto origin = reinterpret_cast<HashBucket*>(hashMemory.seek(z));
    BucketUnion u = *reinterpret_cast<BucketUnion*>(origin);

    if (d < 2 && d < u.v.b[2].getDepth() && u.v.b[2].isAgeMatch(hashAge)) {
        //deep slots are occupied, update only short slot if possible
        assert (d == 0 || d == 1);
        u.v.d[d].set(z, d, n);
        origin->set(3, u.m[3]);
        return;
    }

    //the shallowest deep slot would be overwritten anyway
    if (u.v.b[2].getDepth() < 2) {
        //so we move it into the short slot if possible
        assert (u.v.b[2].getDepth() == 0 || u.v.b[2].getDepth() == 1);
        u.v.d[u.v.b[2].getDepth()].set(u.v.b[2].getKey(), u.v.b[2].getDepth(), u.v.b[2].getNodes());
        origin->set(3, u.m[3]);
    }

    u.v.b[2].set(z, d, n, hashAge);

    if (d < u.v.b[1].getDepth() && u.v.b[1].isAgeMatch(hashAge)) {
        origin->set(2, u.m[2]);
        return;
    }

    origin->set(2, u.m[1]);

    if (d < u.v.b[0].getDepth() && u.v.b[0].isAgeMatch(hashAge)) {
        //move current data in the middle slot
        origin->set(1, u.m[2]);
        return;
    }

    //move current data in the deepest slot
    origin->set(1, u.m[0]);
    origin->set(0, u.m[2]);
}
