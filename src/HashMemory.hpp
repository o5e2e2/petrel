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

    _t  one;

    _t* hash;
    std::uintptr_t mask;

    size_t size;
    size_t max;

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

    _t* lookup(Zobrist z) const;

    const HashAge& getAge() const { return age; }
    void nextAge() { age.next(); }

};

#endif
