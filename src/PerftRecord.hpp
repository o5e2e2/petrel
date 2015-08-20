#ifndef PERFT_RECORD_HPP
#define PERFT_RECORD_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "Zobrist.hpp"
#include "HashBucket.hpp"
#include "StatCounters.hpp"

class PerftRecord {
    typedef unsigned age_t;
    static age_t The_age;

    Zobrist key;
    node_count_t perft;

    enum {DepthShift = 6, AgeShift = 61};

    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xff) << DepthShift;
    static const node_count_t AgeMask = static_cast<node_count_t>(7) << AgeShift;

    static constexpr Zobrist _key(Zobrist::_t z, depth_t d) {
        return Zobrist{ (z & ~DepthMask) | ((static_cast<decltype(z)>(static_cast<unsigned>(d)) << DepthShift) & DepthMask) };
    }

public:
    void set(Zobrist::_t z, depth_t d, node_count_t n) {
        key = _key(z, d);
        setPerft(n);
    }

    void setPerft(node_count_t n) {
        perft = (n & ~AgeMask) | (static_cast<decltype(perft)>(The_age) << AgeShift);
    }

    void updateAge() {
        setPerft(perft);
    }

    bool isKeyMatch(Zobrist::_t z, depth_t d) const {
        return key == _key(z, d);
    }

    bool isOk() const {
        return (static_cast<std::size_t>(perft) >> AgeShift) == The_age;
    }

    depth_t getDepth() const {
        return static_cast<depth_t>(static_cast<std::size_t>(key) >> DepthShift & 0xFF);
    }

    node_count_t getNodes() const {
        return perft & ~AgeMask;
    }

    friend std::ostream& operator << (std::ostream& out, const PerftRecord& r) {
        if (!r.isOk()) { out << '\''; }
        return out << r.getDepth() << ':' << r.getNodes();
    }

    friend bool operator < (const PerftRecord& a, const PerftRecord& b) {
        return a.getNodes() < b.getNodes();
    }

    bool operator <= (node_count_t n) const {
        return getNodes() <= n;
    }

    static void clearAge() {
        The_age = 1;
        stat.clear();
    }

    static void nextAge() {
        //there are 7 ages, not 8, because of:
        //1) need to break 4*n ply transposition pattern
        //2) make sure that initally clear entry is never hidden
        auto a = (The_age + 1) & 7;
        The_age = a? a : 1;
        stat.clear();
    }

};

#endif
