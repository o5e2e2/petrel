#include "PerftTT.hpp"
#include "HashBucket.hpp"
#include "PerftRecord.hpp"
#include "PerftRecordSmall.hpp"

namespace {
    union BucketUnion {
        HashBucket m;
        struct {
            Index<4>::arrayOf<PerftRecordSmall> d;
            Index<2>::arrayOf<PerftRecord> b;
        } v;
    };
}

PerftTT::PerftTT () : hashMemory(sizeof(HashBucket)) {
}

node_count_t PerftTT::get(Zobrist z, ply_t d) {
    ++counter.reads;

    auto origin = reinterpret_cast<HashBucket*>(hashMemory.seek(z));
    BucketUnion u = *reinterpret_cast<BucketUnion*>(origin);

    if (u.v.d[0].isKeyMatch(z, d)) {
        ++counter.hits;
        return u.v.d[0].getNodes();
    }

    if (u.v.d[1].isKeyMatch(z, d)) {
        ++counter.hits;
        return u.v.d[1].getNodes();
    }

    if (u.v.d[2].isKeyMatch(z, d)) {
        ++counter.hits;
        return u.v.d[2].getNodes();
    }

    if (u.v.d[3].isKeyMatch(z, d)) {
        ++counter.hits;
        return u.v.d[3].getNodes();
    }

    if (u.v.b[0].isKeyMatch(z, d)) {
        ++counter.hits;
        auto n = u.v.b[0].getNodes();
        if (d >= u.v.b[1].getDepth()) {
            u.v.b[0].setAge(hashAge);
            origin->set(3, u.m[2]);
            origin->set(2, u.m[3]);
        }
        return n;
    }

    if (u.v.b[1].isKeyMatch(z, d)) {
        ++counter.hits;
        auto n = u.v.b[1].getNodes();
        return n;
    }

    return NodeCountNone;
}

void PerftTT::set(Zobrist z, ply_t d, node_count_t n) {
    ++counter.writes;

    auto origin = reinterpret_cast<HashBucket*>(hashMemory.seek(z));
    BucketUnion u = *reinterpret_cast<BucketUnion*>(origin);

    auto b0d = u.v.b[0].getDepth();

    if (u.v.b[0].isAgeMatch(hashAge) && d < b0d && n <= std::numeric_limits<u32_t>::max() && d <= 0xf) {
        //deep slots are occupied, update only short slot if possible

        if (d == 0) {
            u.v.d[0].set(z, d, n);
            origin->set(0, u.m[0]);
            return;
        }

        if (d == 1) {
            u.v.d[0] = u.v.d[1];
            u.v.d[1].set(z, d, n);
            origin->set(0, u.m[0]);
            return;
        }

        u.v.d[0] = u.v.d[1];
        u.v.d[1] = u.v.d[2];
        origin->set(0, u.m[0]);

        if (d >= u.v.d[3].getDepth()) {
            u.v.d[2] = u.v.d[3];
            u.v.d[3].set(z, d, n);
            origin->set(1, u.m[1]);
            return;
        }

        u.v.d[2].set(z, d, n);
        origin->set(1, u.m[1]);
        return;
    }

    if (b0d <= 4 && u.v.b[0].getNodes() <= std::numeric_limits<u32_t>::max()) {
        //the shallowest deep slot would be overwritten anyway
        //so we move it into the short slot if possible

        u.v.d[0] = u.v.d[1];
        u.v.d[1] = u.v.d[2];
        u.v.d[2] = u.v.d[3];
        u.v.d[3].set(u.v.b[0].getKey(), b0d, u.v.b[0].getNodes());
        origin->set(0, u.m[0]);
        origin->set(1, u.m[1]);
    }

    u.v.b[0].set(z, d, n, hashAge);

    if (u.v.b[1].isAgeMatch(hashAge) && d < u.v.b[1].getDepth()) {
        //move current data in the middle slot
        origin->set(2, u.m[2]);
        return;
    }

    //move current data in the deepest slot
    origin->set(2, u.m[3]);
    origin->set(3, u.m[2]);
}
