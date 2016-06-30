#include "PerftTT.hpp"
#include "SearchInfo.hpp"

node_count_t PerftTT::get(Zobrist z, depth_t d) {
    if (d == 1 && b.s[0].isKeyMatch(z)) {
        return b.s[0].getNodes();
    }

    if (d == 2 && b.s[1].isKeyMatch(z)) {
        return b.s[1].getNodes();
    }

    if (b.b[2].isKeyMatch(z, d)) {
        auto n = b.b[2].getNodes();
        if (b.b[1].getDepth() <= d) {
            b.b[2].updateAge(age);
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
            b.b[1].updateAge(age);
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
    if (b.b[2].isOk(age) && b.b[2].getDepth() > d) {
        if (d == 1) {
            b.s.set(0, z, n);
            origin->set(3, m[3]);
            return;
        }

        if (d == 2) {
            b.s.set(1, z, n);
            origin->set(3, m[3]);
            return;
        }
    }

    if (b.b[2].getDepth() <= 2) {
        if (b.b[2].getDepth() == 1) {
            b.s.set(0, b.b[2].getZobrist(), b.b[2].getNodes());
        }
        else {
            b.s.set(1, b.b[2].getZobrist(), b.b[2].getNodes());
        }
        origin->set(3, m[3]);
    }

    b.b[2].set(z, d, n, age);

    if (b.b[1].isOk(age) && b.b[1].getDepth() > d) {
        origin->set(2, m[2]);
        return;
    }

    origin->set(2, m[1]);

    if (b.b[0].isOk(age) && b.b[0].getDepth() > d) {
        origin->set(1, m[2]);
        return;
    }

    origin->set(1, m[0]);
    origin->set(0, m[2]);
}
