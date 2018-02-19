#ifndef UCI_POSITION_HPP
#define UCI_POSITION_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "PositionMoves.hpp"
#include "Move.hpp"

class UciPosition : public PositionMoves {
    Color colorToMove; //root position color for moves long algebraic format output
    ChessVariant chessVariant; //format of castling moves output

    Move readMove(io::istream&) const;
    void fenEnPassant(io::ostream&) const;

    void setBoard(io::istream&);
    io::istream& setCastling(io::istream&);
    io::istream& setEnPassant(io::istream&);
    void readFen(io::istream&);
    void playMoves(io::istream&);

public:
    constexpr UciPosition() : PositionMoves{}, colorToMove{White}, chessVariant{Orthodox} {}

    constexpr ChessVariant getChessVariant() const { return chessVariant; }
    constexpr void setChessVariant(ChessVariant v) { chessVariant = v; }

    constexpr Color getColorToMove() const { return colorToMove; }
    constexpr const PositionSide& operator[] (Color color) const { return side[colorToMove.is(color)? My : Op];}

    void readUci(io::istream&);
    void setStartpos();

    void limitMoves(io::istream&);

    friend io::ostream& operator << (io::ostream&, const UciPosition&);
};

#endif
