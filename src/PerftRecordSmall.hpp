#ifndef PERFT_RECORD_SMALL_HPP
#define PERFT_RECORD_SMALL_HPP

#include "typedefs.hpp"
#include "HashAge.hpp"
#include "Zobrist.hpp"

class PerftRecordSmall {
    u32_t key;
    u32_t perft;

public:
    void set(Zobrist::_t z, ply_t d, node_count_t n) {
        assert (small_cast<decltype(perft)>(n) == n);

        key   = static_cast<decltype(key)>(z >> 32);
        perft = static_cast<decltype(perft)>(n << 8) | (d & 0xff);

        assert (getNodes() == n);
        assert (getDepth() == d);
    }

    bool isKeyMatch(Zobrist::_t z, ply_t d) const {
        return key == static_cast<decltype(key)>(z >> 32) && d == getDepth();
    }

    Zobrist::_t getKey() const {
        return static_cast<Zobrist::_t>(key) << 40;
    }

    node_count_t getNodes() const {
        return perft >> 8;
    }

    ply_t getDepth() const {
        return perft & 0xff;
    }

};

#endif
