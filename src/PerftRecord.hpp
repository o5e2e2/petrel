#ifndef PERFT_RECORD_HPP
#define PERFT_RECORD_HPP

#include "typedefs.hpp"
#include "HashAge.hpp"
#include "Zobrist.hpp"

class PerftRecord {
    typedef unsigned age_t;

    Zobrist key;
    node_count_t perft;

    enum {DepthShift = 6, AgeShift = 61};

    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xff) << DepthShift;
    static const node_count_t AgeMask = static_cast<node_count_t>(7) << AgeShift;

    static constexpr Zobrist _key(Zobrist::_t z, depth_t d) {
        return Zobrist{ (z & ~DepthMask) | ((static_cast<decltype(z)>(static_cast<unsigned>(d)) << DepthShift) & DepthMask) };
    }

public:
    void set(Zobrist::_t z, depth_t d, node_count_t n, HashAge::_t age) {
        key = _key(z, d);
        setPerft(n, age);
    }

    void setPerft(node_count_t n, HashAge::_t age) {
        perft = (n & ~AgeMask) | (static_cast<decltype(perft)>(age) << AgeShift);
    }

    void updateAge(HashAge::_t age) {
        setPerft(perft, age);
    }

    bool isKeyMatch(Zobrist::_t z, depth_t d) const {
        return key == _key(z, d);
    }

    bool isOk(HashAge::_t age) const {
        return (static_cast<std::size_t>(perft) >> AgeShift) == age;
    }

    depth_t getDepth() const {
        return static_cast<depth_t>(static_cast<std::size_t>(key) >> DepthShift & 0xFF);
    }

    node_count_t getNodes() const {
        return perft & ~AgeMask;
    }

    const Zobrist& getZobrist() const {
        return key;
    }

    friend bool operator < (const PerftRecord& a, const PerftRecord& b) {
        return a.getNodes() < b.getNodes();
    }

    bool operator <= (node_count_t n) const {
        return getNodes() <= n;
    }

};

#endif
