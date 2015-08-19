#ifndef PERFT_RECORD_HPP
#define PERFT_RECORD_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "Zobrist.hpp"
#include "HashBucket.hpp"

class PerftRecord {
    Zobrist key;
    node_count_t perft;

    enum {DepthShift = 6, AgeShift = 61};

    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xFF) << DepthShift;
    static const node_count_t AgeMask = static_cast<node_count_t>(7) << AgeShift;

    static constexpr Zobrist _key(Zobrist::_t z, depth_t d) {
        return Zobrist{ (z & ~DepthMask) | ((static_cast<decltype(z)>(static_cast<unsigned>(d)) << DepthShift) & DepthMask) };
    }

    static constexpr node_count_t _perft(node_count_t n, HashBucket::age_t a) {
        return (n & ~AgeMask) | (static_cast<decltype(n)>(a) << AgeShift);
    }

    void setKey(Zobrist::_t z, depth_t d) {
        this->key = _key(z, d);
    }

    HashBucket::age_t getAge() const {
        return static_cast<HashBucket::age_t>(static_cast<std::size_t>(perft) >> AgeShift);
    }

public:
    void set(Zobrist::_t z, depth_t d, node_count_t n) {
        setKey(z, d);
        setPerft(n);
    }

    void setPerft(node_count_t n) {
        perft = _perft(n, HashBucket::getAge());
    }

    void updateAge() {
        perft = (perft & ~AgeMask) | (static_cast<decltype(perft)>(HashBucket::getAge()) << AgeShift);
    }

    bool isKeyMatch(Zobrist::_t z, depth_t d) const {
        return key == _key(z, d);
    }

    bool isOk() const {
        return getAge() == HashBucket::getAge();
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

};

#endif
