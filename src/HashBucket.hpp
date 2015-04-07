#ifndef HASH_BUCKET_HPP
#define HASH_BUCKET_HPP

#include "bitops128.hpp"
#include "Index.hpp"

class CACHE_ALIGN HashBucket {
public:
    typedef __m128i _t;

private:
    _t  record[4];
    _t* origin;

public:
    constexpr HashBucket () : record{{0,0}, {0,0}, {0,0}, {0,0}}, origin(&record[0]) {}
    constexpr HashBucket (_t* o) : record{o[0], o[1], o[2], o[3]}, origin{o} {}

    void save(Index<4> i) {
        _mm_stream_si128(&origin[i], record[i]);
    }
};

#endif
