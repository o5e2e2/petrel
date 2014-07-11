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

    //UCI command handlers
    void uci();
    void ucinewgame();
    void set_startpos();
    void position();
    void setoption();
    void quit();

    //my own UCI protocol extensions
    void help_and_quit();
    void echo();
    void call(std::ostream& = std::cerr);

    bool operator() (const std::string& filename, std::ostream& = std::cerr);

    void write_nps(std::ostream&);
    void write_info_nps(std::ostream&);

public:
    Uci (SearchControl&, std::ostream& = std::cout);

    bool operator() (int argc, const char* argv[]);
    bool operator() (std::istream& = std::cin, std::ostream& = std::cerr);

    //output from the search
    void write_bestmove(Move move);
    void write_perft_depth(Ply, node_count_t);
    void write_perft_move(Move, index_t, node_count_t);
    void write_info_current();

    //static void write_info_counters(std::ostream&) const;
};

//for debugging
std::ostream& operator << (std::ostream&, Bb);

#endif
