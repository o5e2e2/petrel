#ifndef UCI_CONTROL_HPP
#define UCI_CONTROL_HPP

#include "io.hpp"
#include "SearchControl.hpp"
#include "UciOutput.hpp"

class UciPosition;

class UciControl : public SearchControl {
    UciOutput uciOutput;
public:
    UciControl (const UciPosition&, io::ostream&, io::ostream&);

    void go(io::istream&, const UciPosition&);
    void error(io::istream&) const;
    void info_fen() const;
    void uciok();
    void readyok();
};

#endif
