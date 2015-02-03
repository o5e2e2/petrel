#ifndef UCI_OUTPUT_HPP
#define UCI_OUTPUT_HPP

#include <iosfwd>
#include "SearchOutput.hpp"
#include "Index.hpp"

class SearchControl;
class SearchInfo;
class Position;
class Move;

class UciOutput : public SearchOutput {
    std::ostream& uci_out; //output stream
    const ChessVariant& chessVariant; //format of castling moves output
    const Color& colorToMove; //initial position color for moves long algebraic format output

    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking

    void write(std::ostream&, const Move&) const;
    void nps(std::ostream&, const SearchInfo&) const;
    void info_nps(std::ostream&, const SearchInfo&) const;

public:
    //called from Uci
    UciOutput (std::ostream&, const ChessVariant&, const Color&);
    void isready(const SearchControl&) const;
    void uci(const SearchControl&) const;
    void info_fen(const Position&) const;
    void echo(std::istream&) const;
    void error(std::istream&) const;

    //called from Search
    void report_current(const SearchInfo&) const override;
    void report_bestmove(const SearchInfo&) const override;
    void report_perft_depth(const SearchInfo&) const override;
    void report_perft_divide(const SearchInfo&) const override;
};

#endif
