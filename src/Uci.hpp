#ifndef UCI_HPP
#define UCI_HPP

#include "io.hpp"
#include "SearchControl.hpp"
#include "UciSearchInfo.hpp"
#include "PositionFen.hpp"

class Uci {
    PositionFen positionFen; //root position between 'position' and 'go' commands
    UciSearchInfo info; //output with multithread synchronization
    SearchControl searchControl;

    std::istringstream command; //current input command line
    bool next(io::czstring token) { return io::next(command, token); }
    bool nextNone() { return io::nextNone(command); }

    //UCI command handlers
    void go();
    void position();
    void isready();
    void setHash();
    void setoption();
    void ucinewgame();
    void uciok();

public:
    Uci (io::ostream&);
    void operator() (io::istream&, io::ostream& = std::cerr);
};

#endif
