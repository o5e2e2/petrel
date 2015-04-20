#ifndef UCI_OUTPUT_HPP
#define UCI_OUTPUT_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "SearchOutput.hpp"

class Move;
class Position;
class SearchControl;
class SearchInfo;
class UciHash;

class UciOutput : public SearchOutput {
    std::ostream& out; //output stream
    const UciHash& uciHash;
    const ChessVariant& chessVariant; //format of castling moves output
    const Color& colorToMove; //initial position color for moves long algebraic format output

    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking

    void write(std::ostream&, const Move&) const;
    void hashfull(std::ostream&) const;
    void nps(std::ostream&, const SearchInfo&) const;
    void info_nps(std::ostream&, const SearchInfo&) const;

public:
    //called from Uci
    UciOutput (std::ostream&, const UciHash&, const ChessVariant&, const Color&);
    void isready(const SearchControl&) const;
    void uciok() const;
    void info_fen(const Position&) const;
    void echo(std::istream&) const;
    void error(std::istream&) const;
    void error(const std::string&) const;

    //called from Search
    void readyok(const SearchInfo&) const override;
    void bestmove(const SearchInfo&) const override;
    void info_depth(const SearchInfo&) const override;
    void info_currmove(const SearchInfo&) const override;
};

#endif
