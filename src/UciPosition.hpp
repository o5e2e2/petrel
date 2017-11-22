#ifndef UCI_POSITION_HPP
#define UCI_POSITION_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "PositionMoves.hpp"
#include "Move.hpp"

class UciPosition : public PositionMoves {
    Color colorToMove; //root position color for moves long algebraic format output
    ChessVariant chessVariant; //format of castling moves output

    Move readMove(std::istream&) const;
    void fenEnPassant(std::ostream&) const;

    void setBoard(std::istream&);
    std::istream& setCastling(std::istream&);
    std::istream& setEnPassant(std::istream&);
    void readFen(std::istream&);
    void playMoves(std::istream&);

public:
    UciPosition() : PositionMoves{}, colorToMove{White}, chessVariant{Orthodox} {}

    ChessVariant getChessVariant() const { return chessVariant; }
    void setChessVariant(ChessVariant v) { chessVariant = v; }

    Color getColorToMove() const { return colorToMove; }

    const PositionSide& getSide(Color color) const { return side[colorToMove.is(color)? My : Op];}

    void readUci(std::istream&);
    void setStartpos();

    void limitMoves(std::istream&);

    friend std::ostream& operator << (std::ostream&, const UciPosition&);
};

#endif
