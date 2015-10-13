#include "PerftTT.hpp"
#include "SearchInfo.hpp"

node_count_t PerftTT::get(Zobrist z, depth_t d, SearchInfo& info) {
    ++info[TT_Tried];

    if (d == 1) {
        if (b.s[0].isKeyMatch(z)) {
            auto n = b.s[0].getNodes();
            ++info[TT_Hit];

            return n;
        }
        if (b.s[1].isKeyMatch(z)) {
            auto n = b.s[1].getNodes();
            ++info[TT_Hit];

            b.s.swap();
            origin.save(3, m[3]);
            return n;
        }
    }

    if (b.b[0].isKeyMatch(z, d)) {
        auto n = b.b[0].getNodes();
        ++info[TT_Hit];

        if (!b.b[1].isOk(age) || b.b[1].getDepth() <= d) {
            Index i = 1;
            b.b[0].updateAge(age);
            origin.save(0, m[1]);
            if (!b.b[2].isOk(age) || b.b[2].getDepth() <= d) {
                i = 2;
                origin.save(1, m[2]);
            }
            origin.save(i, m[0]);
            return n;
        }
        if (!b.b[0].isOk(age)) {
            b.b[0].updateAge(age);
            origin.save(0, m[0]);
        }
        return n;
    }

    if (b.b[1].isKeyMatch(z, d)) {
        auto n = b.b[1].getNodes();
        ++info[TT_Hit];

        if (!b.b[2].isOk(age) || b.b[2].getDepth() <= d) {
            b.b[1].updateAge(age);
            origin.save(1, m[2]);
            origin.save(2, m[1]);
            return n;
        }
        if (!b.b[1].isOk(age)) {
            b.b[1].updateAge(age);
            origin.save(1, m[1]);
        }
        return n;
    }

    if (b.b[2].isKeyMatch(z, d)) {
        auto n = b.b[2].getNodes();
        ++info[TT_Hit];

        if (!b.b[2].isOk(age)) {
            b.b[2].updateAge(age);
            origin.save(2, m[2]);
        }
        return n;
    }

    return 0;
}

void PerftTT::set(Zobrist z, depth_t d, node_count_t n, SearchInfo&) {
    if (d == 1 && b.b[0].isOk(age) && b.b[0].getDepth() > 1) {
        if (!b.s[0].isKeyMatch(z)) {
            b.s.set(z, n);
            origin.save(3, m[3]);
        }
        return;
    }

    if (b.b[0].getDepth() == 1) {
        /*if (!b.s[0].isKeyMatch(b.b[0].getZobrist()))*/ {
            b.s.set(b.b[0].getZobrist(), b.b[0].getNodes());
        }
    }

    Index i = 0;
    if (!b.b[1].isOk(age) || b.b[1].getDepth() <= d) {
        b.b[0] = b.b[1];
        i = 1;
        if (!b.b[2].isOk(age) || b.b[2].getDepth() <= d) {
            b.b[1] = b.b[2];
            i = 2;
        }
    }
    b.b[i].set(z, d, n, age);
    origin = m;
}
