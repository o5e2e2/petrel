#include "PerftTT.hpp"
#include "SearchInfo.hpp"

node_count_t PerftTT::get(Zobrist z, depth_t d) {
    if (d == 1) {
        if (b.s[0].isKeyMatch(z)) {
            return b.s[0].getNodes();
        }
        if (b.s[1].isKeyMatch(z)) {
            auto n = b.s[1].getNodes();
            b.s.swap();
            origin.save(3, m[3]);
            return n;
        }
    }

    if (b.b[0].isKeyMatch(z, d)) {
        auto n = b.b[0].getNodes();
        if (b.b[1].getDepth() <= d) {
            b.b[0].updateAge(age);
            if (b.b[2].getDepth() <= d) {
                origin.save(2, m[0]);
                origin.save(1, m[2]);
            }
            else {
                origin.save(1, m[0]);
            }
            origin.save(0, m[1]);
        }
        return n;
    }

    if (b.b[1].isKeyMatch(z, d)) {
        auto n = b.b[1].getNodes();
        if (b.b[2].getDepth() <= d) {
            b.b[1].updateAge(age);
            origin.save(1, m[2]);
            origin.save(2, m[1]);
        }
        return n;
    }

    if (b.b[2].isKeyMatch(z, d)) {
        auto n = b.b[2].getNodes();
        return n;
    }

    return NODE_COUNT_NONE;
}

void PerftTT::set(Zobrist z, depth_t d, node_count_t n) {
    if (d == 1 && b.b[0].isOk(age) && b.b[0].getDepth() > 1) {
        b.s.set(z, n);
        origin.save(3, m[3]);
        return;
    }
    if (b.b[0].getDepth() == 1) {
        b.s.set(b.b[0].getZobrist(), b.b[0].getNodes());
        origin.save(3, m[3]);
    }

    b.b[0].set(z, d, n, age);

    if (!b.b[1].isOk(age) || b.b[1].getDepth() <= d) {
        if (!b.b[2].isOk(age) || b.b[2].getDepth() <= d) {
            origin.save(2, m[0]);
            origin.save(1, m[2]);
        }
        else {
            origin.save(1, m[0]);
        }
        origin.save(0, m[1]);
    }
    else {
        origin.save(0, m[0]);
    }
}
