#ifndef UCI_HPP
#define UCI_HPP

#include "io.hpp"
#include "PositionMoves.hpp"
#include "UciControl.hpp"
#include "UciOutput.hpp"

class Uci {
    UciOutput uciOutput;
    PositionMoves rootPosition; //initial chess position and moves to analyze
    UciControl uciControl;

    std::istringstream command; //current input command line

    bool next(io::literal token) { return io::next(command, token); }

    //UCI command handlers
    void go();
    void setoption();
    void startpos();
    void position();
    void ucinewgame();
    void quit();

public:
    Uci (std::ostream&, std::ostream&);
    void operator() (std::istream&);
};

#endif
