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

    Color colorToMove;  //root position color needed for moves input/output

    SearchControl searchControl;
    SearchLimit searchLimit;

    Position rootPosition; //initial chess position to analyze
    PositionMoves rootMoves; //'go searchmoves'

    mutable std::istringstream command; //current input command line

    bool next(io::literal token) const { return io::next(command, token); }

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
