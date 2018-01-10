#ifndef UCI_CONTROL_HPP
#define UCI_CONTROL_HPP

#include "SearchControl.hpp"

class UciOutput;
class UciPosition;

class UciControl : public SearchControl {
public:
    UciControl (UciOutput& o);

    void go(io::istream&, const UciPosition&);
    void uciok();
    void readyok();
};

#endif
