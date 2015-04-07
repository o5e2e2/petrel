#ifndef HASH_BUCKET_HPP
#define HASH_BUCKET_HPP

#include "bitops128.hpp"
#include "Index.hpp"

class CACHE_ALIGN HashBucket {
public:
    typedef __m128i _t;
    typedef ::Index<4> Index;

private:
    _t  record[Index::Size];
    _t* origin;

    constexpr static _t* x(_t* o, Index i) {
        return ::xor_ptr(o, i);
    }

public:
    static const size_t Size = sizeof(record);

    constexpr HashBucket () : record{{0,0}, {0,0}, {0,0}, {0,0}, /*{0,0}, {0,0}, {0,0}, {0,0}*/}, origin(&record[0]) {}
    constexpr HashBucket (_t* o) : record{*x(o, 0), *x(o, 1), *x(o, 2), *x(o, 3), /* *x(o, 4), *x(o, 5), *x(o, 6), *x(o, 7)*/}, origin{o} {}

    void save(Index i) {
        _mm_stream_si128(x(origin, i), record[i]);
    }
};

#endif
