#ifndef UCI_HPP
#define UCI_HPP

#include "io.hpp"
#include "UciControl.hpp"
#include "UciPosition.hpp"

class Uci {
    UciPosition uciPosition; //store position just between 'position' and 'go' commands
    UciControl uciControl;

    std::istringstream command; //current input command line

    bool next(io::literal_type token) { return io::next(command, token); }

    //UCI command handlers
    void go();
    void setoption();
    void startpos();
    void position();
    void ucinewgame();
    void quit();

public:
    Uci (io::ostream&, io::ostream&);
    void operator() (io::istream&);
};

#endif
