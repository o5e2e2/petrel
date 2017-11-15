#ifndef UCI_CONTROL_HPP
#define UCI_CONTROL_HPP

#include "io.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "UciPosition.hpp"

class UciOutput;

class UciControl : public SearchControl {
    UciPosition position;
    SearchLimit searchLimit;

public:
     UciControl (UciOutput& o);

    void go(std::istream&, const UciPosition&);
    void readUciHash(std::istream&);

    void uciok();
    void readyok();
};

#endif
