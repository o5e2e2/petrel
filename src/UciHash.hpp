#ifndef UCI_HASH_HPP
#define UCI_HASH_HPP

#include <cstddef>
#include "io.hpp"
#include "HashMemory.hpp"
#include "PerftTT.hpp"

class UciHash {
public:
    typedef unsigned _t;

private:
    enum { MiB = 1024 * 1024 };
    HashMemory& hashMemory;

    static _t toMiB(HashMemory::size_t bytes) { return small_cast<_t>(bytes / MiB); }

public:
    UciHash (HashMemory& hash) : hashMemory(hash) {}
    void resize(_t mb) {
        hashMemory.resize(static_cast<HashMemory::size_t>(mb) * MiB);
        PerftTT::resetAge();
    }

    void newGame() {
        hashMemory.clear();
        PerftTT::resetAge();
    }

    std::ostream& option(std::ostream& out) const {
        auto current = hashMemory.getSize();
        auto max = hashMemory.getMax();

        out << "option name Hash type spin min 0 max " << toMiB(max) << " default " << toMiB(current) << '\n';
        return out;
    }

    unsigned long hashfull() const {
        return (static_cast<unsigned long>(PerftTT::getUsed())*1000) / (hashMemory.getSize() * 4 / HashMemory::ClusterSize);
    }
};

#endif
