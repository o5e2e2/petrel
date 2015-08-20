#include "HashMemory.hpp"
#include "bitops.hpp"
#include "memory.hpp"
#include "PerftRecord.hpp"

namespace {
    template <typename T>
    T round(T n) {
        assert (n > 0);
        return ::singleton<decltype(n)>(::bsr(n));
    }
}

void HashMemory::clear() {
    ::memset(hash, 0, size);
    PerftRecord::clearAge();
}

void HashMemory::set(_t* _hash, size_t _size) {
    assert (_size == round(_size));

    hash = _hash;
    size = _size;
    mask = (size-1) ^ (BucketSize-1);

    clear();
}

void HashMemory::setDefault() {
    set(&one, BucketSize);
}

void HashMemory::free() {
    if (size == BucketSize) {
        clear();
        return;
    }

    auto p = this->hash;
    setDefault();
    ::freeAligned(p);
}

void HashMemory::resize(size_t bytes) {
    bytes = (bytes <= BucketSize)? BucketSize : round(bytes);

    if (bytes == size) {
        clear();
        return;
    }

    free();
    max = round(::getAvailableMemory());

    for (bytes = std::min(bytes, max); bytes > BucketSize; bytes >>= 1) {
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
