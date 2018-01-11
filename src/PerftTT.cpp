#include "PerftTT.hpp"

node_count_t PerftTT::get(Zobrist z, depth_t d) {
    if (d < 2 && b.s[d].isKeyMatch(z)) {
        return b.s[d].getNodes();
    }

    if (b.b[2].isKeyMatch(z, d)) {
        auto n = b.b[2].getNodes();
        if (b.b[1].getDepth() <= d) {
            b.b[2].updateAge(hashAge);
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
            b.b[1].updateAge(hashAge);
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
    if (d < 2 && b.b[2].isOk(hashAge) && b.b[2].getDepth() > d) {
        b.s.set(d, z, n);
        origin->set(3, m[3]);
        return;
    }

    if (b.b[2].getDepth() < 2) {
        b.s.set(b.b[2].getDepth(), b.b[2].getZobrist(), b.b[2].getNodes());
        origin->set(3, m[3]);
    }

    b.b[2].set(z, d, n, hashAge);

    if (b.b[1].isOk(hashAge) && b.b[1].getDepth() > d) {
        origin->set(2, m[2]);
        return;
    }

    origin->set(2, m[1]);

    if (b.b[0].isOk(hashAge) && b.b[0].getDepth() > d) {
        origin->set(1, m[2]);
        return;
    }

    origin->set(1, m[0]);
    origin->set(0, m[2]);
}
