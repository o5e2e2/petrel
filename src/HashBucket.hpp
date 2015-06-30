#ifndef HASH_BUCKET_HPP
#define HASH_BUCKET_HPP

#include "bitops128.hpp"
#include "Index.hpp"

class CACHE_ALIGN HashBucket {
public:
    typedef ::Index<4> Index;

private:
    typedef __m128i _t;
    _t record[Index::Size];

public:
    constexpr HashBucket() : record {{0,0}, {0,0}, {0,0}, {0,0}} {}

    constexpr HashBucket(HashBucket* p)
        : record {
            reinterpret_cast<_t*>(p)[0],
            reinterpret_cast<_t*>(p)[1],
            reinterpret_cast<_t*>(p)[2],
            reinterpret_cast<_t*>(p)[3]
        } {}

    void save(Index index, __m128i data) {
        _mm_stream_si128(&reinterpret_cast<_t*>(this)[index], data);
    }

};

#endif
