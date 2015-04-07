#ifndef HASH_MEMORY_HPP
#define HASH_MEMORY_HPP

#include "bitops128.hpp"
#include "HashBucket.hpp"
#include "Zobrist.hpp"

class HashMemory {
public:
    typedef std::size_t size_t;

private:

    HashBucket  one;
    HashBucket* hash;

    size_t mask;
    size_t size;
    size_t max;

    HashMemory (const HashMemory&) = delete;
    HashMemory& operator = (const HashMemory&) = delete;

    static size_t round(size_t bytes);

    void set(HashBucket*, size_t);
    void setOne();
    void free();

    void setMax();

public:
    static const size_t BucketSize = sizeof(HashBucket);

    HashMemory () { setOne(); setMax(); }
   ~HashMemory () { free(); }

    void resize(size_t bytes);
    void clear();

    size_t getSize() const { return size; }
    size_t getMask() const { return mask; }
    size_t getMax()  const { return max; }

    HashBucket::_t* lookup(Zobrist z) const {
        auto p = reinterpret_cast<char*>(hash) + (static_cast<Zobrist::_t>(z) & mask);
        _mm_prefetch(p, _MM_HINT_T1);
        return reinterpret_cast<HashBucket::_t*>(p);
    }

};

#endif
