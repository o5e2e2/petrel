#ifndef HASH_MEMORY_HPP
#define HASH_MEMORY_HPP

#include "bitops128.hpp"
#include "HashBucket.hpp"
#include "Zobrist.hpp"

class HashMemory {
public:
    typedef std::size_t size_t;
    typedef HashBucket::_t _t;

private:

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

    _t* lookup(Zobrist z) const {
        auto o = reinterpret_cast<char*>(hash) + (static_cast<Zobrist::_t>(z) & mask);
        _mm_prefetch(o, _MM_HINT_T0);

        if (HashBucket::Size > 0100) {
            _mm_prefetch(::xor_ptr<char, 0100>(o, 1), _MM_HINT_T0);
            if (HashBucket::Size > 0200) {
                _mm_prefetch(::xor_ptr<char, 0100>(o, 2), _MM_HINT_T0);
                _mm_prefetch(::xor_ptr<char, 0100>(o, 3), _MM_HINT_T0);
            }
        }

        return reinterpret_cast<_t*>(o);
    }

};

#endif
