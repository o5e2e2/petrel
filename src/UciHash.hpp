#ifndef UCI_HASH_HPP
#define UCI_HASH_HPP

#include <cstddef>
#include "HashMemory.hpp"

class UciHash {
    enum { MiB = 1024 * 1024 };
    HashMemory& hashMemory;

public:
    typedef unsigned _t;
    UciHash (HashMemory& hash) : hashMemory(hash) {}

    void clear() { hashMemory.clear(); }
    void setSize(_t mb) { hashMemory.setSize(static_cast<std::size_t>(mb) * MiB); }

    _t getSize()    const { return small_cast<_t>(hashMemory.getSize() / MiB); }
    _t getMaxSize() const { return small_cast<_t>(hashMemory.getMaxSize() / MiB); }
};

#endif
