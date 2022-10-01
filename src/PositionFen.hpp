#ifndef POSITION_FEN_HPP
#define POSITION_FEN_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "PositionMoves.hpp"
#include "Move.hpp"

class PositionFen : public PositionMoves {
    Color colorToMove = White; //root position color for moves long algebraic format output
    ChessVariant chessVariant = Orthodox; //format of castling moves output

    io::istream& setBoard(io::istream&);
    io::istream& setCastling(io::istream&);
    io::istream& setEnPassant(io::istream&);
    io::istream& readMove(io::istream&, Square&, Square&) const;

    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    bool setEnPassant(File);

public:
    constexpr PositionFen() = default;

    constexpr Side sideOf(Color color) const { return colorToMove.is(color) ? My : Op; }
    constexpr bool isChess960() const { return chessVariant.is(Chess960); }

    void setChessVariant(ChessVariant v) { chessVariant = v; }
    void setStartpos();
    void readFen(io::istream&);
    void playMoves(io::istream&);
    void limitMoves(io::istream&);

    friend io::ostream& operator << (io::ostream&, const PositionFen&);
    void write(io::ostream&, Move) const;
    void write(io::ostream&, const Move[]) const;
};

#endif
