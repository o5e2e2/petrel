#ifndef HASH_MEMORY_HPP
#define HASH_MEMORY_HPP

#include "HashBucket.hpp"
#include "Zobrist.hpp"

class HashMemory {
    HashMemory (const HashMemory&) = delete;
    HashMemory& operator = (const HashMemory&) = delete;

public:
    typedef std::size_t size_t;
    typedef HashBucket _t;

private:
    enum : size_t { BucketSize = sizeof(_t) };

    _t  one;

    _t* hash;
    std::uintptr_t mask;

    size_t size;
    size_t max;

    void set(_t*, size_t);
    void setDefault();
    void free();

public:
    HashMemory () { setDefault(); }
   ~HashMemory () { free(); }

    size_t getMax()  const { return max; }
    size_t getSize() const { return size; }
    size_t getTotalRecords() const { return (getSize() / HashBucket::getBucketSize()) * HashBucket::getBucketCount(); }

    void resize(size_t bytes);
    void clear();

    _t* lookup(Zobrist z) const;

};

#endif
