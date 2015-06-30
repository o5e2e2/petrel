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

    size_t mask;
    size_t size;

    static size_t round(size_t bytes);

    void set(_t*, size_t);
    void setOne();
    void free();

public:
    HashMemory () { setOne(); }
   ~HashMemory () { free(); }

    static size_t getMax();
    size_t getSize() const { return size; }
    size_t getTotalRecords() const { return (size / BucketSize) * 4; }

    void resize(size_t bytes);
    void clear();

    _t* lookup(Zobrist z) const;

};

#endif
