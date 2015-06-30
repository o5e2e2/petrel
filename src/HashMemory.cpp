#include "HashMemory.hpp"
#include "bitops.hpp"
#include "memory.hpp"

HashMemory::size_t HashMemory::getMax() {
    return round(::getAvailableMemory());
}

HashMemory::size_t HashMemory::round(size_t bytes) {
    return (bytes > 0)? ::singleton<decltype(bytes)>(::bsr(bytes)) : 0;
}

void HashMemory::clear() {
    ::memset(hash, 0, size);
}

void HashMemory::set(_t* _hash, size_t _size) {
    assert (_size == round(_size));

    ::memset(_hash, 0, _size);
    hash = _hash;
    size = _size;
    mask = (size-1) ^ (BucketSize-1);
}

void HashMemory::setOne() {
    set(&one, sizeof(one));
}

void HashMemory::free() {
    if (size == BucketSize) {
        clear();
    }
    else {
        auto p = this->hash;
        setOne();

        ::freeAligned(p);
    }
}

void HashMemory::resize(size_t bytes) {
    bytes = (bytes <= BucketSize)? BucketSize : round(bytes);

    if (bytes == size) {
        clear();
        return;
    }

    free();

    for (bytes = std::min(bytes, getMax()); bytes > BucketSize; bytes >>= 1) {
        auto p = reinterpret_cast<_t*>(::allocateAligned(bytes, BucketSize));

        if (p) {
            set(p, bytes);
            return;
        }
    }

}

HashMemory::_t* HashMemory::lookup(Zobrist z) const {
    auto o = reinterpret_cast<char*>(hash) + (static_cast<Zobrist::_t>(z) & mask);

    enum { CACHE_PAGES = (BucketSize-1)/64 + 1 };
    for (index_t i = 0; i < CACHE_PAGES; i++) {
        _mm_prefetch(o + i, _MM_HINT_T0);
    }

    return reinterpret_cast<_t*>(o);
}
