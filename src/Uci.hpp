#ifndef UCI_HPP
#define UCI_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "UciOutput.hpp"

class Uci {
    UciOutput uciOutput;

    SearchControl searchControl;
    SearchLimit searchLimit;

    Position rootPosition; //initial chess position to analyze
    PositionMoves rootMoves; //'go searchmoves'

    std::istringstream command; //current input command line

    bool next(io::literal token) { return io::next(command, token); }

    //UCI command handlers
    void go();
    void setoption();
    void startpos();
    void position();
    void ucinewgame();
    void quit();

    //UCI protocol extensions
    void call();
    void echo();

public:
    Uci (std::ostream&, std::ostream&);
    void operator() (std::istream&);
    void call(const std::string& filename);
};

#endif
