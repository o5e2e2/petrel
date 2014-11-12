#ifndef UCI_HPP
#define UCI_HPP

#include <iostream>
#include <sstream>

#include "io.hpp"
#include "Position.hpp"
#include "SearchLimit.hpp"
#include "UciOutput.hpp"

class SearchControl;

class Uci {
    Position start_position; //initial chess position to analyze
    SearchLimit limit;
    UciOutput output;

    SearchControl& search;
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
    void read_go_limits(); //defined in SearchLimits.cpp

    //my own UCI protocol extensions
    void echo();
    void call();

    bool next(io::literal);

public:
    Uci (SearchControl&, std::ostream& = std::cout);
    bool operator() (const std::string& filename);
    bool operator() (std::istream& = std::cin);

};

#endif
