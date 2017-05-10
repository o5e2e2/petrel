#ifndef UCI_OUTPUT_HPP
#define UCI_OUTPUT_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "SearchInfo.hpp"
#include "SpinLock.hpp"

class HashMemory;
class Move;
class Position;

class UciOutput : public SearchInfo {
    std::ostream& out; //output stream
    std::ostream& err; //error output stream

    Color colorToMove; //root position color for moves long algebraic format output
    ChessVariant chessVariant; //format of castling moves output

    mutable SpinLock outLock;
    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking
    mutable node_count_t lastInfoNodes;

    void write(std::ostream&, const Move&) const;
    void hashfull(std::ostream&) const;
    void nps(std::ostream&) const;
    void info_nps(std::ostream&) const;

public:
    UciOutput (std::ostream&, std::ostream& = std::cerr);

    //called from Uci
    void isready(bool) const;
    void uciok(const HashMemory&) const;
    void info_fen(const Position&) const;
    void set(ChessVariant v) { chessVariant = v; }
    void setColorToMove(Color c) { colorToMove = c; }
    Color getColorToMove() const { return colorToMove; }

    void error(std::istream&) const;
    void error(const std::string&) const;

    //called from Search
    void readyok() const override;
    void bestmove() const override;
    void info_depth() const override;
    void info_currmove() const override;

};

#endif
