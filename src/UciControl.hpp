#ifndef UCI_CONTROL_HPP
#define UCI_CONTROL_HPP

#include "io.hpp"
#include "SearchControl.hpp"

class UciOutput;

class UciControl : public SearchControl {
public:
    UciControl (UciOutput& o);

    void readUciHash(std::istream&);

    void uciok();
    void readyok();
};

#endif
