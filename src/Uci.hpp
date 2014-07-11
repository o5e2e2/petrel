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
    std::ostream& uci_err; //error output stream

    volatile bool isready_waiting; //set when got 'isready' command while thinking

    //UCI command handlers
    void uci();
    void ucinewgame();
    void isready();
    void go();
    void set_startpos();
    void position();
    void setoption();

    //my own UCI protocol extensions
    void echo();
    void call();

    void log_error();

public:
    Uci (SearchControl&, std::ostream& = std::cout, std::ostream& = std::cerr);

    bool operator() (const std::string& filename);
    bool operator() (std::istream& = std::cin);

    void write_info_current();
    void write(std::ostream&, Move) const;

    void report_bestmove(Move);
    void report_perft_depth(Ply, node_count_t);
    void report_perft(Move, index_t currmovenumber, node_count_t);
};

std::ostream& program_version(std::ostream&);

//debug output
std::ostream& operator << (std::ostream&, Bb);
std::ostream& operator << (std::ostream&, VectorPiSquare);

#endif
