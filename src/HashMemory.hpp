#ifndef HASH_MEMORY_HPP
#define HASH_MEMORY_HPP

#include "HashBucket.hpp"
#include "HashAge.hpp"
#include "Zobrist.hpp"

class HashMemory {
    HashMemory (const HashMemory&) = delete;
    HashMemory& operator = (const HashMemory&) = delete;

public:
    typedef std::size_t size_t;
    typedef HashBucket _t;

    struct Info {
        size_t current;
        size_t max;
    };

private:
    enum : size_t { BucketSize = sizeof(_t) };
    const size_t PageSize;

    _t  one;

    _t* hash;
    std::uintptr_t mask;

    size_t size;
    size_t max;

    struct {
        size_t reads = 0;
        size_t writes = 0;
        size_t hits = 0;
    } counter;

    HashAge age;

    void set(_t*, size_t);
    void setDefault();
    void free();

public:
    HashMemory ();
   ~HashMemory () { free(); }

    Info getInfo() const {
        return {size, max};
    }

    size_t getTotalRecords() const { return (size / BucketSize) * _t::Index::Size; }

    void resize(size_t bytes);
    void clear();

    _t* prefetch(Zobrist z) const;

    const HashAge& getAge() const { return age; }
    void nextAge() { age.next(); }

    void countRead() {
        ++counter.reads;
    }

    void countWrite() {
        ++counter.writes;
    }

    void countHit() {
        ++counter.hits;
    }

    size_t getReads() const {
        return counter.reads;
    }

    size_t getWrites() const {
        return counter.writes;
    }

    size_t getHits() const {
        return counter.hits;
    }

};

#endif
