#ifndef PERFT_RECORD_SMALL_HPP
#define PERFT_RECORD_SMALL_HPP

#include "typedefs.hpp"
#include "HashAge.hpp"
#include "Zobrist.hpp"

class PerftRecordSmall {
    u32_t key;
    u32_t perft;

public:
    void set(Zobrist::_t z, node_count_t n) {
        assert (small_cast<decltype(perft)>(n) == n);

        key   = small_cast<decltype(key)>(z >> 32);
        perft = small_cast<decltype(perft)>(n);
    }

    bool isKeyMatch(Zobrist::_t z) const {
        return key == small_cast<decltype(key)>(z >> 32);
    }

    node_count_t getNodes() const {
        return perft;
    }

};

#endif
