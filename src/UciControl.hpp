#ifndef UCI_CONTROL_HPP
#define UCI_CONTROL_HPP

#include "SearchControl.hpp"

class UciOutput;

class UciControl : public SearchControl {
public:
    UciControl (UciOutput& o);

    void uciok();
    void readyok();
};

#endif
