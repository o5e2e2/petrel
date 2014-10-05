#ifndef UCI_HPP
#define UCI_HPP

#include <iostream>
#include <sstream>

#include "SearchOutput.hpp"

#include "io.hpp"
#include "Move.hpp"
#include "Position.hpp"
#include "SearchLimit.hpp"

class SearchControl;
class SearchLimit;

class Uci : public SearchOutput {
    Position start_position; //initial chess position to analyze
    SearchLimit search_limit;

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
    void setoption();
    void isready();

    void position();
    void set_startpos();

    void go();
    void read_go_limits();

    //my own UCI protocol extensions
    void echo();
    void call();

    bool next(io::literal);

    void log_error();
    void write(std::ostream&, Move) const;
    void nps(std::ostream&) const;
    void info_nps(std::ostream&) const;

public:
    Uci (SearchControl&, std::ostream& = std::cout, std::ostream& = std::cerr);
    bool operator() (const std::string& filename);
    bool operator() (std::istream& = std::cin);

    void write_info_current() override;
    void report_bestmove(Move) override;
    void report_perft_depth(depth_t, node_count_t) override;
    void report_perft(Move, index_t currmovenumber, node_count_t) override;
};

#endif
