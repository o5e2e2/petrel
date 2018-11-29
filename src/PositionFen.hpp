#ifndef POSITION_FEN_HPP
#define POSITION_FEN_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "PositionMoves.hpp"
#include "Move.hpp"

class PositionFen : public PositionMoves {
    Color colorToMove; //root position color for moves long algebraic format output
    ChessVariant chessVariant; //format of castling moves output

    Move readMove(io::istream&) const;
    void fenEnPassant(io::ostream&) const;

    void setBoard(io::istream&);
    io::istream& setCastling(io::istream&);
    io::istream& setEnPassant(io::istream&);

public:
    constexpr PositionFen() : PositionMoves{}, colorToMove{White}, chessVariant{Orthodox} {}

    constexpr ChessVariant& getChessVariant() { return chessVariant; }
    constexpr void setChessVariant(ChessVariant v) { chessVariant = v; }

    constexpr Color getColorToMove() const { return colorToMove; }
    constexpr Color& getColorToMove() { return colorToMove; }

    void setStartpos();
    void readFen(io::istream&);
    void playMoves(io::istream&);

    void limitMoves(io::istream&);

    friend io::ostream& operator << (io::ostream&, const PositionFen&);
};

#endif
