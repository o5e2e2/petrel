#ifndef UCI_HPP
#define UCI_HPP

#include <iostream>
#include <sstream>

#include "io.hpp"
#include "Position.hpp"
#include "SearchLimit.hpp"
#include "OutputUci.hpp"

class SearchControl;

class Uci {
    Position start_position; //initial chess position to analyze
    SearchLimit search_limit;
    SearchControl& search;
    OutputUci output;

    ChessVariant chessVariant; //format of castling moves output
    Color colorToMove; //initial position color for long algebraic format moves output

    std::istringstream command; //current input command line
    std::ostream& uci_err; //error output stream

    //UCI command handlers
    void ucinewgame();
    void setoption();

    void position();
    void set_startpos();

    void go();
    void read_go_limits(Color);

    //my own UCI protocol extensions
    void echo();
    void call();

    bool next(io::literal);

    void log_error();

public:
    Uci (SearchControl&, std::ostream& = std::cout, std::ostream& = std::cerr);
    bool operator() (const std::string& filename);
    bool operator() (std::istream& = std::cin);

};

#endif
