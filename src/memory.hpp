#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "typedefs.hpp"

using std::size_t;

size_t getAvailableMemory();
size_t getPageSize();

void* allocateAligned(size_t size, size_t alignment);
void  freeAligned(void*);

#endif
