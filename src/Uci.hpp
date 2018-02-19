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

    bool next(io::literal_type token) { return io::next(command, token); }

    //UCI command handlers
    void go();
    void setoption();
    void startpos();
    void position();
    void ucinewgame();
    void setHash();
    void quit();

public:
    Uci (io::ostream&, io::ostream&);
    void operator() (io::istream&);
};

#endif
