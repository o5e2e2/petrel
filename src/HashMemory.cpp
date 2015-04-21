#include "HashMemory.hpp"
#include "bitops.hpp"
#include "memory.hpp"

void HashMemory::setMax() {
    this->max = round(::getAvailableMemory());
}

HashMemory::size_t HashMemory::round(size_t bytes) {
    return (bytes > 0)? ::singleton<decltype(bytes)>(::bsr(bytes)) : 0;
}

void HashMemory::clear() {
    ::memset(this->hash, 0, this->size);
}

void HashMemory::set(_t* _hash, size_t _size) {
    ::memset(_hash, 0, _size);

    this->hash = _hash;
    this->size = _size;

    assert (size == round(size));
    this->mask = (size-1) ^ (sizeof(_t)-1);
}

void HashMemory::setOne() {
    set(reinterpret_cast<_t*>(&one), ClusterSize);
}

void HashMemory::resize(size_t bytes) {
    bytes = (bytes <= ClusterSize)? ClusterSize : round(bytes);

    if (bytes == this->size) {
        clear();
        return;
    }

    free();
    setMax();

    bytes = std::min(bytes, getMax());

    for (; bytes > ClusterSize; bytes >>= 1) {
        auto p = ::allocateAligned(bytes, ClusterSize);

        if (p) {
            set(reinterpret_cast<_t*>(p), bytes);
            return;
        }
    }

}

void HashMemory::free() {
    if (this->size > ClusterSize) {
        auto p = this->hash;
        setOne();

        ::freeAligned(p);
    }
}
