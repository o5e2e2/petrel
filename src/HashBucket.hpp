#ifndef HASH_BUCKET_HPP
#define HASH_BUCKET_HPP

#include "bitops128.hpp"
#include "Index.hpp"

class CACHE_ALIGN HashBucket {
public:
    typedef ::Index<4> Index;
    typedef i128_t _t;

private:
    Index::arrayOf<_t> _v;

public:
    constexpr HashBucket() : _v{{{0,0}, {0,0}, {0,0}, {0,0}}} {}
    constexpr const _t& operator[] (Index i) const { return _v[i]; }

    HashBucket& operator = (const HashBucket& a) {
        _mm_stream_si128(&_v[0], a[0]);
        _mm_stream_si128(&_v[1], a[1]);
        _mm_stream_si128(&_v[2], a[2]);
        _mm_stream_si128(&_v[3], a[3]);
        return *this;
    }

    void set(Index i, _t m) {
        _mm_stream_si128(&_v[i], m);
    }

};

#endif
