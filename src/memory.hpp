#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstring>

std::size_t getAvailableMemory();

void* allocateAligned(std::size_t size, std::size_t alignment);
void  freeAligned(void*);

using std::memset;

#endif
