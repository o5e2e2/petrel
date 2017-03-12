#ifndef UCI_OUTPUT_HPP
#define UCI_OUTPUT_HPP

#include <mutex>
#include "io.hpp"
#include "typedefs.hpp"
#include "SearchOutput.hpp"

class Move;
class Position;
class SearchControl;
class SearchInfo;

class UciOutput : public SearchOutput {
    std::ostream& out; //output stream
    std::ostream& err; //error output stream

    mutable std::mutex outputLock;
    typedef std::lock_guard<decltype(outputLock)> Lock;

    Color colorToMove; //root position color for moves long algebraic format output
    ChessVariant chessVariant; //format of castling moves output

    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking

    void write(std::ostream&, const Move&) const;
    void hashfull(std::ostream&) const;
    void nps(std::ostream&, const SearchInfo&) const;
    void info_nps(std::ostream&, const SearchInfo&) const;

public:
    UciOutput (std::ostream&, std::ostream& = std::cerr);

    //called from Uci
    void isready(const SearchControl&) const;
    void uciok(const SearchControl&) const;
    void info_fen(const Position&) const;
    void set(ChessVariant v) { chessVariant = v; }
    void setColorToMove(Color c) { colorToMove = c; }
    Color getColorToMove() const { return colorToMove; }

    void error(std::istream&) const;
    void error(const std::string&) const;

    //called from Search
    void readyok(const SearchInfo&) const override;
    void bestmove(const SearchInfo&) const override;
    void info_depth(const SearchInfo&) const override;
    void info_currmove(const SearchInfo&) const override;

};

#endif
