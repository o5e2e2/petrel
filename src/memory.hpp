#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstring>

using std::size_t;
using std::memset;

size_t getAvailableMemory();

void* allocateAligned(size_t size, size_t alignment);
void  freeAligned(void*);

#endif
