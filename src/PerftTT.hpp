#ifndef Perft_TT_HPP
#define Perft_TT_HPP

#include "Zobrist.hpp"
#include "HashMemory.hpp"

class PerftTT {
    static const Zobrist::_t DepthMask = static_cast<Zobrist::_t>(0xFF) << 6;

    struct PerftRecord {
        Zobrist::_t key;
        node_count_t perft;
    };

    PerftRecord* record;

public:
    PerftTT(char* c) : record(reinterpret_cast<decltype(record)>(c)) {}

    node_count_t get(Zobrist z, depth_t d) const {
        Zobrist::_t key = (static_cast<Zobrist::_t>(z) & ~DepthMask) | ((d << 6) & DepthMask);
        return (record->key == key)? record->perft : 0;
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
        auto key = (static_cast<Zobrist::_t>(z) & ~DepthMask) | ((d << 6) & DepthMask);
        record->key = key;
        record->perft = n;
    }
};

#endif
