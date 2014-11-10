#ifndef OUTPUT_UCI_HPP
#define OUTPUT_UCI_HPP

#include <iosfwd>
#include "OutputSearch.hpp"

class SearchControl;
class Position;

class OutputUci : public OutputSearch {
    std::ostream& uci_out; //output stream
    volatile bool isready_waiting; //set when got 'isready' command while thinking

    const SearchControl& search;
    const ChessVariant& chessVariant; //format of castling moves output
    const Color& colorToMove; //initial position color for moves long algebraic format output

    void write(std::ostream&, Move) const;
    void nps(std::ostream&) const;
    void info_nps(std::ostream&) const;

public:
    OutputUci (std::ostream&, const SearchControl&, const ChessVariant&, const Color&);

    //called from Search
    void write_info_current() override;
    void report_bestmove(Move) override;
    void report_perft_depth(depth_t, node_count_t) override;
    void report_perft(Move, index_t currmovenumber, node_count_t) override;

    //called from Uci
    void uci();
    void isready();
    void info_fen(const Position&);
    void echo(std::istream&);
};

#endif
