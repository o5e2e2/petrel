#ifndef HASH_MEMORY_HPP
#define HASH_MEMORY_HPP

#include "HashBucket.hpp"
#include "Zobrist.hpp"

class HashMemory {
public:
    typedef std::size_t size_t;
    typedef HashBucket::_t _t;

private:
    static const size_t ClusterSize = HashBucket::Size;

    HashBucket one;
    _t* hash;

    size_t mask;
    size_t size;
    size_t max;

    HashMemory (const HashMemory&) = delete;
    HashMemory& operator = (const HashMemory&) = delete;

    static size_t round(size_t bytes);

    void set(_t*, size_t);
    void setOne();
    void free();

    void setMax();

public:
    HashMemory () { setOne(); setMax(); }
   ~HashMemory () { free(); }

    void resize(size_t bytes);
    void clear();

    size_t getSize() const { return size; }
    size_t getMax()  const { return max; }
    size_t getTotalRecords() const { return getSize() / sizeof(_t); }

    _t* lookup(Zobrist z) const;

};

#endif
