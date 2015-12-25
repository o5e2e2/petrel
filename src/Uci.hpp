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
    SearchControl searchControl;

    Position rootPosition; //initial chess position to analyze
    Color colorToMove; //initial position color for long algebraic format moves output

    PositionMoves searchMoves;
    SearchLimit searchLimit;

    UciOutput uciOutput;

    mutable std::istringstream command; //current input command line
    bool next(io::literal keyword) const { return io::next(command, keyword); }

    //UCI command handlers
    void go();
    void setoption();
    void startpos();
    void position();
    void ucinewgame();
    void quit() const;

    //UCI protocol extensions
    void call();
    void echo() const;

public:
    Uci (std::ostream&);
    void operator() (std::istream&);
    void call(const std::string& filename);
};

#endif
