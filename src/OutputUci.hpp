#ifndef OUTPUT_UCI_HPP
#define OUTPUT_UCI_HPP

#include <iosfwd>
#include "OutputSearch.hpp"

class SearchControl;

class OutputUci : public OutputSearch {
    friend class Uci;

    SearchControl& search;
    std::ostream& uci_out; //output stream

    ChessVariant chessVariant; //format of castling moves output
    Color colorToMove; //initial position color for moves long algebraic format output
    volatile bool isready_waiting; //set when got 'isready' command while thinking

    void write(std::ostream&, Move) const;
    void nps(std::ostream&) const;
    void info_nps(std::ostream&) const;

public:
    OutputUci (SearchControl&, std::ostream&);

    void write_info_current() override;
    void report_bestmove(Move) override;
    void report_perft_depth(depth_t, node_count_t) override;
    void report_perft(Move, index_t currmovenumber, node_count_t) override;

    void isready();
};

#endif
