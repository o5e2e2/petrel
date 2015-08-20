#ifndef STAT_COUNTERS_HPP
#define STAT_COUNTERS_HPP

#include "typedefs.hpp"

enum { TT_Tried, TT_Hit, TT_Used, _Total };

class StatCounters {
public:
    typedef node_count_t _t;
    typedef ::Index<_Total> Index;

private:
    Index::array<_t> _v;

public:
    constexpr StatCounters () : _v{0, 0, 0} {}
    void clear() { *this = {}; }

    _t get(Index i) const { return _v[i]; }
    void inc(Index i) { ++_v[i]; }

};

extern StatCounters stat;

#endif
