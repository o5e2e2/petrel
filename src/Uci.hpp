#ifndef UCI_HPP
#define UCI_HPP

#include "io.hpp"
#include "SearchControl.hpp"
#include "UciPosition.hpp"
#include "UciSearchInfo.hpp"

class Uci {
    UciPosition uciPosition; //store position just between 'position' and 'go' commands
    UciSearchInfo uciSearchInfo;
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
