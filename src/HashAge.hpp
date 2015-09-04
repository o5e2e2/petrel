#ifndef HASH_AGE_HPP
#define HASH_AGE_HPP

class HashAge {
public:
    typedef unsigned _t;

private:
    _t _v;

public:
    constexpr HashAge () : _v(1) {}
    constexpr operator const _t& () { return _v; }

    void next() {
        //there are 7 ages, not 8, because of:
        //1) the need to break 4*n ply transposition pattern
        //2) make sure that initally clear entry is never hidden
        auto a = (_v + 1) & 7;
        _v = a? a : 1;
    }

};

#endif
