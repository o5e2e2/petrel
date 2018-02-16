#include "PerftTT.hpp"

node_count_t PerftTT::get(Zobrist z, depth_t d) {
    if (d < 2 && b.s[d].isKeyMatch(z)) {
        return b.s[d].getNodes();
    }

    if (b.b[2].isKeyMatch(z, d)) {
        auto n = b.b[2].getNodes();
        if (b.b[1].getDepth() <= d) {
            b.b[2].setAge(hashAge);
            if (b.b[0].getDepth() <= d) {
                origin->set(0, m[2]);
                origin->set(1, m[0]);
            }
            else {
                origin->set(1, m[2]);
            }
            origin->set(2, m[1]);
        }
        return n;
    }

    if (b.b[1].isKeyMatch(z, d)) {
        auto n = b.b[1].getNodes();
        if (b.b[0].getDepth() <= d) {
            b.b[1].setAge(hashAge);
            origin->set(1, m[0]);
            origin->set(0, m[1]);
        }
        return n;
    }

    if (b.b[0].isKeyMatch(z, d)) {
        auto n = b.b[0].getNodes();
        return n;
    }

    return NODE_COUNT_NONE;
}

void PerftTT::set(Zobrist z, depth_t d, node_count_t n) {
    if (d < 2 && d < b.b[2].getDepth() && b.b[2].isAgeMatch(hashAge)) {
        //deep slots are occupied, update only short slot if possible
        assert (d == 0 || d == 1);
        b.s.set(d, z, n);
        origin->set(3, m[3]);
        return;
    }

    //the shallowest deep slot would be overwritten anyway
    if (b.b[2].getDepth() < 2) {
        //so we move it into the short slot if possible
        assert (b.b[2].getDepth() == 0 || b.b[2].getDepth() == 1);
        b.s.set(b.b[2].getDepth(), b.b[2].getKey(), b.b[2].getNodes());
        origin->set(3, m[3]);
    }

    b.b[2].set(z, d, n, hashAge);

    if (d < b.b[1].getDepth() && b.b[1].isAgeMatch(hashAge)) {
        origin->set(2, m[2]);
        return;
    }

    origin->set(2, m[1]);

    if (d < b.b[0].getDepth() && b.b[0].isAgeMatch(hashAge)) {
        //move current data in the middle slot
        origin->set(1, m[2]);
        return;
    }

    //move current data in the deepest slot
    origin->set(1, m[0]);
    origin->set(0, m[2]);
}
