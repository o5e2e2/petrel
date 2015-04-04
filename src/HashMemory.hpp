#ifndef HASH_MEMORY_HPP
#define HASH_MEMORY_HPP

#include <new>
#include "Vector.hpp"
#include "Zobrist.hpp"

class HashMemory {
public:
    typedef std::size_t size_t;

private:
    class CACHE_ALIGN Cluster {
        __m128i _t[4];
    };

    Cluster one;
    Cluster* hash;

    size_t mask;
    size_t size;
    size_t max;
    static const size_t ClusterSize = sizeof(Cluster);

    HashMemory (const HashMemory&) = delete;
    HashMemory& operator = (const HashMemory&) = delete;

    static size_t round(size_t bytes);

    void set(Cluster*, size_t);
    void setOne();
    void free();

    void setMax();

public:
    HashMemory () { set(&one, ClusterSize); setMax(); }
   ~HashMemory () { free(); }

    void resize(size_t bytes);

    size_t getSize() const { return size; }
    size_t getMask() const { return mask; }
    size_t getMax()  const { return max; }

    char* lookup(Zobrist z) const {
        auto p = &(reinterpret_cast<char*>(hash)[static_cast<Zobrist::_t>(z) & mask]);
        _mm_prefetch(p, _MM_HINT_T1);
        return p;
    }

};

#endif
