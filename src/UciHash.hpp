#ifndef UCI_HASH_HPP
#define UCI_HASH_HPP

#include "io.hpp"

class HashMemory;

class UciHash {
public:
    typedef unsigned _t;

private:
    HashMemory& hashMemory;

public:
    UciHash (HashMemory& hash);
    void resize(_t mb);
    void newGame();

    std::ostream& option(std::ostream& out) const;
    std::ostream& hashfull(std::ostream& out) const;
};

#endif
