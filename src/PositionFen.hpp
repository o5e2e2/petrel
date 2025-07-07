#ifndef POSITION_FEN_HPP
#define POSITION_FEN_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "PositionMoves.hpp"
#include "Move.hpp"

using in::istream;
using out::ostream;

class PositionFen : public PositionMoves {
    Color colorToMove = White; //root position color for moves long algebraic format output
    ChessVariant chessVariant = Orthodox; //format of castling moves output

    istream& readBoard(istream&);
    istream& readCastling(istream&);
    istream& readEnPassant(istream&);
    istream& readMove(istream&, Square&, Square&) const;

    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    bool setEnPassant(File);

public:
    constexpr Side sideOf(Color color) const { return colorToMove.is(color) ? My : Op; }
    constexpr bool isChess960() const { return chessVariant.is(Chess960); }

    void setChessVariant(ChessVariant v) { chessVariant = v; }
    void setStartpos();
    void readFen(istream&);
    void playMoves(istream&);
    void limitMoves(istream&);

    friend ostream& operator << (ostream&, const PositionFen&);
    ostream& write(ostream&, Move, ply_t = 0) const;
    ostream& write(ostream&, const Move[], ply_t = 0) const;
};

#endif
