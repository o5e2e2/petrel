#ifndef UCI_HPP
#define UCI_HPP

#include <iostream>
#include <sstream>

#include "io.hpp"
#include "Position.hpp"
#include "SearchLimit.hpp"
#include "SearchControl.hpp"
#include "UciOutput.hpp"

class SearchControl;

class Uci {
    SearchControl searchControl;
    Position startPosition; //initial chess position to analyze
    SearchLimit goLimit;
    UciOutput output;

    ChessVariant chessVariant; //format of castling moves output
    Color colorToMove; //initial position color for long algebraic format moves output

    std::istringstream command; //current input command line

    //UCI command handlers
    void ucinewgame();
    void setoption();
    void isready();

    void position();
    void set_startpos();

    void go();

    //my own UCI protocol extensions
    void echo();
    void call();

    bool next(io::literal keyword) { return io::next(command, keyword); }

public:
    Uci (std::ostream& = std::cout);
    bool operator() (const std::string& filename);
    bool operator() (std::istream& = std::cin);

};

#endif
