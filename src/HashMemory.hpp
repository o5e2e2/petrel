#ifndef HASH_MEMORY_HPP
#define HASH_MEMORY_HPP

#include <new>
#include "Vector.hpp"
#include "Zobrist.hpp"

class HashMemory {
    class CACHE_ALIGN Cluster {
        __m128i _t[4];
    };

    Cluster one;
    Cluster* hash;

    typedef std::size_t size_type;
    size_type mask;
    size_type size;
    static const size_type ClusterSize = sizeof(Cluster);

    HashMemory (const HashMemory&) = delete;
    HashMemory& operator = (const HashMemory&) = delete;

    static size_type round(size_type bytes);

    void set(Cluster*, size_type);
    void free();

public:
    HashMemory () { set(&one, ClusterSize); }
   ~HashMemory () { free(); }

    void clear() { std::memset(hash, 0, size); }
    void setSize(size_type bytes);

    size_type getSize() const { return size; }
    static size_type getMaxSize();

    char* lookup(Zobrist z) const {
        auto p = &(reinterpret_cast<char*>(hash)[static_cast<Zobrist::_t>(z) & mask]);
        _mm_prefetch(p, _MM_HINT_T1);
        return p;
    }

};

#endif
