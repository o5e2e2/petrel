#ifndef UCI_HASH_HPP
#define UCI_HASH_HPP

#include <cstddef>
#include "TranspositionTable.hpp"

class UciHash {
    enum { MiB = 1024 * 1024 };
    TranspositionTable& tt;

public:
    typedef unsigned _t;
    UciHash (TranspositionTable& _tt) : tt(_tt) {}

    void clear() { tt.clear(); }
    void setSize(_t mb) { tt.setSize(static_cast<std::size_t>(mb) * MiB); }

    _t getSize()    const { return small_cast<_t>(tt.getSize() / MiB); }
    _t getMaxSize() const { return small_cast<_t>(tt.getMaxSize() / MiB); }
};

#endif
