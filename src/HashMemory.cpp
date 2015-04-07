#include <new>
#include "HashMemory.hpp"
#include "bitops.hpp"

#ifdef _WIN32

#include <windows.h>
std::size_t getAvailableMemory() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    status.ullAvailPhys = 0;

    GlobalMemoryStatusEx(&status);
    return status.ullAvailPhys;
}

#else

#include <unistd.h>
std::size_t getAvailableMemory() {
    auto pages = sysconf(_SC_AVPHYS_PAGES);
    auto page_size = sysconf(_SC_PAGE_SIZE);
    return static_cast<std::size_t>(pages) * static_cast<std::size_t>(page_size);
}

#endif

void HashMemory::setMax() {
    this->max = round(::getAvailableMemory());
}

HashMemory::size_t HashMemory::round(size_t bytes) {
    return (bytes > 0)? ::singleton<decltype(bytes)>(::bsr(bytes)):0;
}

void HashMemory::clear() {
    std::memset(this->hash, 0, this->size);
}

void HashMemory::set(_t* _hash, size_t _size) {
    std::memset(_hash, 0, _size);

    this->hash = _hash;
    this->size = _size;

    assert (size == round(size));
    this->mask = (size-1) ^ (sizeof(_t)-1);
}

void HashMemory::setOne() {
    set(reinterpret_cast<_t*>(&one), HashBucket::Size);
}

void HashMemory::resize(size_t bytes) {
    bytes = (bytes <= HashBucket::Size)? HashBucket::Size : round(bytes);

    if (bytes == this->size) {
        return;
    }

    free();
    setMax();

    bytes = std::min(bytes, getMax());

    auto records = bytes / sizeof(_t);
    for (; records > HashBucket::Size/sizeof(_t); records >>= 1) {
        auto p = new (std::nothrow) _t[records];

        if (p) {
            set(p, static_cast<size_t>(records) * sizeof(_t));
            return;
        }
    }

}

void HashMemory::free() {
    if (this->size > HashBucket::Size) {
        auto toFree = this->hash;
        setOne();

        delete[] toFree;
    }
}
