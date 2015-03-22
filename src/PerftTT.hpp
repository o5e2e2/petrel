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

    static Zobrist::_t key(Zobrist z, depth_t d) {
        return (static_cast<Zobrist::_t>(z) & ~DepthMask) | ((d << 6) & DepthMask);
    }

public:
    PerftTT(char* c) : record(reinterpret_cast<decltype(record)>(c)) {}

    node_count_t get(Zobrist z, depth_t d) const {
        return (record->key == key(z, d))? record->perft : 0;
    }

    void set(Zobrist z, depth_t d, node_count_t n) {
        record->key = key(z, d);
        record->perft = n;
    }
};

#endif
