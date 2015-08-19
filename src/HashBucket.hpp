#ifndef HASH_BUCKET_HPP
#define HASH_BUCKET_HPP

#include "bitops128.hpp"
#include "Index.hpp"

class CACHE_ALIGN HashBucket {
public:
    typedef ::Index<4> Index;
    typedef __m128i _t;

    typedef unsigned age_t;
    typedef node_count_t used_t;

    struct Counter {
        used_t tried;
        used_t hit;
        used_t used;
        age_t age;

        constexpr Counter () : tried{0}, hit{0}, used{0}, age{0} {}
    };
    static Counter counter;

private:
    Index::array<_t> _v;

public:
    constexpr HashBucket() : _v{{{0,0}, {0,0}, {0,0}, {0,0}}} {}
    constexpr HashBucket(const HashBucket& a) = default;
    constexpr const _t& operator[] (Index i) const { return _v[i]; }

    void save(Index i, __m128i m) {
        _mm_stream_si128(&_v[i], m);
    }

    void save(__m128i a[4]) {
        _mm_stream_si128(&_v[0], a[0]);
        _mm_stream_si128(&_v[1], a[1]);
        _mm_stream_si128(&_v[2], a[2]);
        _mm_stream_si128(&_v[3], a[3]);
    }

    constexpr static index_t getBucketCount() { return Index::Size; }
    constexpr static index_t getBucketSize()  { return sizeof(HashBucket); }

    static void clearAge() {
        counter = {};
        counter.age = 1;
    }

    static void nextAge() {
        //there are 7 ages, not 8, because of:
        //1) need to break 4*n ply transposition pattern
        //2) make sure that initally clear entry is never hidden
        auto a = (counter.age + 1) & 7;
        counter = {};
        counter.age = a? a : 1;
    }

    static node_count_t getUsed()  { return counter.used; }
    static node_count_t getHit()   { return counter.hit; }
    static node_count_t getTried() { return counter.tried; }
    static age_t        getAge()   { return counter.age; }

    static void tickUsed()  { ++counter.used; }
    static void tickHit()   { ++counter.hit; }
    static void tickTried() { ++counter.tried; }

};

#endif
