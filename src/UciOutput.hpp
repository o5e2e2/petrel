#ifndef UCI_OUTPUT_HPP
#define UCI_OUTPUT_HPP

#include <iosfwd>
#include "SearchOutput.hpp"
#include "Move.hpp"

class SearchControl;
class SearchInfo;
class Position;

class UciOutput : public SearchOutput {
    std::ostream& uci_out; //output stream
    const ChessVariant& chessVariant; //format of castling moves output
    const Color& colorToMove; //initial position color for moves long algebraic format output

    volatile bool isready_waiting; //set when got 'isready' command while thinking

    void write(std::ostream&, Move) const;
    void nps(std::ostream&, const SearchInfo&) const;
    void info_nps(std::ostream&, const SearchInfo&) const;

public:
    //called from Uci
    UciOutput (std::ostream&, const ChessVariant&, const Color&);
    void uci(const SearchControl&);
    void isready(bool);
    void info_fen(const Position&);
    void echo(std::istream&);
    void error(std::istream&);

    //called from Search
    void report_current(const SearchInfo&) override;
    void report_bestmove(const SearchInfo&) override;
    void report_perft_depth(const SearchInfo&) override;
    void report_perft_divide(const SearchInfo&) override;
};

#endif
