#include "HashMemory.hpp"
#include "bitops.hpp"

#ifdef _WIN32

#include <windows.h>
std::size_t getFreeMemory() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    status.ullAvailPhys = 0;

    GlobalMemoryStatusEx(&status);
    return status.ullAvailPhys;
}

#else

#include <unistd.h>
std::size_t getFreeMemory() {
    auto pages = sysconf(_SC_AVPHYS_PAGES);
    auto page_size = sysconf(_SC_PAGE_SIZE);
    return static_cast<std::size_t>(pages) * static_cast<std::size_t>(page_size);
}

#endif

HashMemory::size_type HashMemory::round(size_type bytes) {
    return (bytes > 0)? ::singleton<decltype(bytes)>(::bsr(bytes)):0;
}

HashMemory::size_type HashMemory::getMaxSize() {
    auto available = getFreeMemory();
    return round(available);
}

void HashMemory::set(Cluster* _hash, size_type _size) {
    hash = _hash;
    size = _size;

    assert (size == round(size));
    mask = (size-1) ^ (ClusterSize-1);

    clear();
}

void HashMemory::setSize(size_type bytes) {
    bytes = (bytes <= ClusterSize)? ClusterSize : round(bytes);

    if (size == bytes) {
        return;
    }

    free();

    bytes = std::min(bytes, getMaxSize());
    for (; bytes > ClusterSize; bytes >>= 1) {
        auto p = reinterpret_cast<decltype(hash)>(new (std::nothrow) Cluster[bytes/ClusterSize]);

        if (p) {
            set(p, bytes);
            return;
        }
    }

}

void HashMemory::free() {
    if (size > ClusterSize) {
        delete[] hash;
    }

    set(&one, ClusterSize);
}
