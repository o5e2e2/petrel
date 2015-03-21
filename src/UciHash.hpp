#ifndef UCI_HASH_HPP
#define UCI_HASH_HPP

#include <cstddef>
#include "io.hpp"
#include "HashMemory.hpp"

class UciHash {
public:
    typedef unsigned _t;

private:
    enum { MiB = 1024 * 1024 };
    HashMemory& hashMemory;

    _t getSize()    const { return small_cast<_t>(hashMemory.getSize() / MiB); }
    _t getMaxSize() const { return small_cast<_t>(hashMemory.getMaxSize() / MiB); }

public:
    UciHash (HashMemory& hash) : hashMemory(hash) {}

    void clear() { hashMemory.clear(); }
    void setSize(_t mb) { hashMemory.setSize(static_cast<std::size_t>(mb) * MiB); }

    std::ostream& option(std::ostream& out) const {
        auto currentMiB = getSize();
        auto maxMiB = getMaxSize();

        out << "option name Hash type spin min 0 max " << maxMiB << " default " << currentMiB << '\n';
        return out;
    }
};

#endif
