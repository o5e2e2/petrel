#include "PerftTT.hpp"
#include "SearchInfo.hpp"

void PerftTT::popup(::Index<3> i) {
    //move the i entry to the actual age region, reordering the rest
    b.b[i].updateAge(age);
    auto d = b.b[i].getDepth();

    for (::Index<3> j = ::Index<3>::Size-1; j > i; --j) {
        //seek the new slot for i
        if (!b.b[j].isOk(age) || b.b[j].getDepth() <= d) {
            for (auto k = i; k < j; ++k) {
                origin.save(+k, m[k+1]);
            }
            origin.save(+j, m[+i]);
            return;
        }
    }

    origin.save(+i, m[+i]);
    return;
}

node_count_t PerftTT::get(Zobrist z, depth_t d, SearchInfo& info) {
    ++info[TT_Tried];

    FOR_INDEX(::Index<3>, i) {
        if (b.b[i].isKeyMatch(z, d)) {
            ++info[TT_Hit];
            popup(i);
            return b.b[i].getNodes();
        }
    }

    if (d == 1) {
        if (b.s[0].isKeyMatch(z)) {
            ++info[TT_Hit];
            return b.s[0].getNodes();
        }
        if (b.s[1].isKeyMatch(z)) {
            b.s.swap();
            origin.save(3, m[3]);
            ++info[TT_Hit];
            return b.s[0].getNodes();
        }
    }

    return 0;
}

void PerftTT::set(Zobrist z, depth_t d, node_count_t n, SearchInfo&) {
    /*
    //replace the same entry
    FOR_INDEX(::Index<3>, i) {
        if (b.b[i].isKeyMatch(z, d)) {
            b.b[i].setPerft(n, age);
            popup(i);
            return;
        }
    }
    */

    if (d == 1) {
        if (b.b[0].isOk(age) && b.b[0].getDepth() > 1) {
            if (!b.s[0].isKeyMatch(z)) {
                b.s.set(z, n);
                origin.save(3, m[3]);
            }
            return;
        }
    }

    if (b.b[0].getDepth() == 1) {
        /*if (!b.s[0].isKeyMatch(b.b[0].getZobrist()))*/ {
            b.s.set(b.b[0].getZobrist(), b.b[0].getNodes());
            origin.save(3, m[3]);
        }
    }

    Index i = 0;
    if (!b.b[1].isOk(age) || b.b[1].getDepth() <= d) {
        origin.save(0, m[1]);
        i = 1;
        if (!b.b[2].isOk(age) || b.b[2].getDepth() <= d) {
            origin.save(1, m[2]);
            i = 2;
        }
    }
    b.b[i].set(z, d, n, age);
    origin.save(i, m[i]);
}
