#ifndef UCI_HPP
#define UCI_HPP

#include "io.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "PositionFen.hpp"
#include "UciSearchInfo.hpp"

class Uci {
    PositionFen positionFen; //root position between 'position' and 'go' commands
    SearchLimit searchLimit;
    UciSearchInfo info;
    SearchControl searchControl;

    std::istringstream command; //current input command line
    bool next(io::czstring token) { return io::next(command, token); }

    //UCI command handlers
    void ucinewgame();
    void setoption();
    void position();
    void go();

public:
    Uci (io::ostream&);
    void operator() (io::istream&, io::ostream& = std::cerr);
};

#endif
