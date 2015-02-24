#include "TranspositionTable.hpp"
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

TranspositionTable::size_type TranspositionTable::round(size_type bytes) {
    return (bytes > 0)? ::singleton<decltype(bytes)>(::bsr(bytes)):0;
}

void TranspositionTable::setMaxSize() {
    auto available = getFreeMemory();
    available = round(available);
    maxSize = available;
}
