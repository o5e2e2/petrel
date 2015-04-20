#ifndef UCI_HPP
#define UCI_HPP

#include "io.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "UciHash.hpp"
#include "UciOutput.hpp"

class Uci {
    SearchControl searchControl;
    UciHash uciHash;
    UciOutput uciOutput;
    Position rootPosition; //initial chess position to analyze

    PositionMoves searchMoves;
    SearchLimit goLimit;

    ChessVariant chessVariant; //format of castling moves output
    Color colorToMove; //initial position color for long algebraic format moves output

    mutable std::istringstream command; //current input command line
    bool next(io::literal keyword) const { return io::next(command, keyword); }

    //UCI command handlers
    void go();
    void setoption();
    void startpos();
    void position();
    void ucinewgame();

    //UCI protocol extensions
    void call();
    void echo() const;
    int  exit() const;

public:
    Uci (std::ostream&);
    int operator() (std::istream&);
    int call(const std::string& filename);
};

#endif
