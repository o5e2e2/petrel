#include <cstdlib>

#include "memory.hpp"

#ifdef _WIN32

#include <windows.h>
std::size_t getAvailableMemory() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    status.ullAvailPhys = 0;

    ::GlobalMemoryStatusEx(&status);
    return status.ullAvailPhys;
}

#else

#include <unistd.h>
std::size_t getAvailableMemory() {
    std::size_t pages     = ::sysconf(_SC_AVPHYS_PAGES);
    std::size_t page_size = ::sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}

#endif

void* allocateAligned(std::size_t size, std::size_t alignment) {
#ifdef _ISOC11_SOURCE
    return ::aligned_alloc(alignment, size);
#else
#   if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
    void* result = nullptr;
    ::posix_memalign(&result, alignment, size);
    return result;
#   endif
#endif
}

void freeAligned(void* p) {
    std::free(p);
}
