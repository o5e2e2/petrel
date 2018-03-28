#include "PerftTT.hpp"
#include "HashBucket.hpp"
#include "PerftRecord.hpp"
#include "PerftRecordDual.hpp"

namespace {
    typedef Index<3>::array<PerftRecord> Bucket;

    union BucketUnion {
        HashBucket m;
        struct {
            Bucket b;
            PerftRecordDual s;
        } b;
    };
}

PerftTT::PerftTT () : hashMemory(sizeof(HashBucket)) {
}

node_count_t PerftTT::get(Zobrist z, depth_t d) {
    ++counter.reads;

    auto origin = reinterpret_cast<HashBucket*>(hashMemory.seek(z));
    BucketUnion u = *reinterpret_cast<BucketUnion*>(origin);

    if (d < 2 && u.b.s[d].isKeyMatch(z)) {
        ++counter.hits;
        return u.b.s[d].getNodes();
    }

    if (u.b.b[2].isKeyMatch(z, d)) {
        ++counter.hits;
        auto n = u.b.b[2].getNodes();
        if (u.b.b[1].getDepth() <= d) {
            u.b.b[2].setAge(hashAge);
            if (u.b.b[0].getDepth() <= d) {
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

    if (u.b.b[1].isKeyMatch(z, d)) {
        ++counter.hits;
        auto n = u.b.b[1].getNodes();
        if (u.b.b[0].getDepth() <= d) {
            u.b.b[1].setAge(hashAge);
            origin->set(1, u.m[0]);
            origin->set(0, u.m[1]);
        }
        return n;
    }

    if (u.b.b[0].isKeyMatch(z, d)) {
        ++counter.hits;
        auto n = u.b.b[0].getNodes();
        return n;
    }

    return NodeCountNone;
}

void PerftTT::set(Zobrist z, depth_t d, node_count_t n) {
    ++counter.writes;

    auto origin = reinterpret_cast<HashBucket*>(hashMemory.seek(z));
    BucketUnion u = *reinterpret_cast<BucketUnion*>(origin);

    if (d < 2 && d < u.b.b[2].getDepth() && u.b.b[2].isAgeMatch(hashAge)) {
        //deep slots are occupied, update only short slot if possible
        assert (d == 0 || d == 1);
        u.b.s.set(d, z, n);
        origin->set(3, u.m[3]);
        return;
    }

    //the shallowest deep slot would be overwritten anyway
    if (u.b.b[2].getDepth() < 2) {
        //so we move it into the short slot if possible
        assert (u.b.b[2].getDepth() == 0 || u.b.b[2].getDepth() == 1);
        u.b.s.set(u.b.b[2].getDepth(), u.b.b[2].getKey(), u.b.b[2].getNodes());
        origin->set(3, u.m[3]);
    }

    u.b.b[2].set(z, d, n, hashAge);

    if (d < u.b.b[1].getDepth() && u.b.b[1].isAgeMatch(hashAge)) {
        origin->set(2, u.m[2]);
        return;
    }

    origin->set(2, u.m[1]);

    if (d < u.b.b[0].getDepth() && u.b.b[0].isAgeMatch(hashAge)) {
        //move current data in the middle slot
        origin->set(1, u.m[2]);
        return;
    }

    //move current data in the deepest slot
    origin->set(1, u.m[0]);
    origin->set(0, u.m[2]);
}
