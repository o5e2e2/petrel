#ifndef PERFT_TT_HPP
#define PERFT_TT_HPP

#include "HashAge.hpp"
#include "HashMemory.hpp"
#include "Zobrist.hpp"

class HashBucket;

class PerftTT {
    HashMemory hashMemory;
    HashAge hashAge;

    struct Counter {
        size_t reads = 0;
        size_t writes = 0;
        size_t hits = 0;
    } counter;

public:
    PerftTT ();

    node_count_t get(Zobrist, ply_t);
    void set(Zobrist, ply_t, node_count_t);

    const Counter& getCounter() const { return counter; }

    const HashAge& getAge() const { return hashAge; }
    void nextAge() { hashAge.nextAge(); }

    const HashInfo& getInfo() const { return hashMemory.getInfo(); }
    void resize(size_t bytes) { hashMemory.resize(bytes); }
    void clear() { hashMemory.clear(); clearCounter(); hashAge = {}; }
    void clearCounter() { counter = {0, 0, 0}; }

};

#endif
