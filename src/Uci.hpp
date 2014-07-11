#ifndef UCI_HPP
#define UCI_HPP

#include <iostream>
#include <sstream>

#include "io.hpp"
#include "typedefs.hpp"
#include "Index.hpp"
#include "Move.hpp"
#include "Position.hpp"

class SearchControl;

class Uci {
    Position start_position; //initial chess position to analyze

    SearchControl& search;
    ChessVariant chessVariant; //format of castling moves output
    Color colorToMove; //initial position color for moves long algebraic format output

    std::istringstream command; //current input command line
    std::ostream& uci_out; //output stream

    volatile bool isready_waiting; //set when got 'isready' command while thinking

    //UCI command handlers
    void uci();
    void ucinewgame();
    void isready();
    void go();
    void set_startpos();
    void position();
    void setoption();
    void quit();

    //my own UCI protocol extensions
    void help_and_quit();
    void echo();
    void call(std::ostream& = std::cerr);

    bool operator() (const std::string& filename, std::ostream& = std::cerr);

public:
    Uci (SearchControl&, std::ostream& = std::cout);

    bool operator() (int argc, const char* argv[]);
    bool operator() (std::istream& = std::cin, std::ostream& = std::cerr);

    void write_info_current();
    void write(std::ostream&, Move) const;

};

//debug output
std::ostream& operator << (std::ostream&, Bb);
std::ostream& operator << (std::ostream&, VectorPiSquare);

#endif
