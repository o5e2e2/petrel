#ifndef POSITION_HPP
#define POSITION_HPP

#include "PositionSide.hpp"

class Position {
protected:
    Side::array<PositionSide> ps;

private:
    template <Side::_t> void updateSliderAttacks(VectorPiMask);
    template <Side::_t> void updateSliderAttacks(VectorPiMask, VectorPiMask);
    template <Side::_t> void setLegalEnPassant(Pi);
    template <Side::_t> void capture(Square);
    template <Side::_t> void playKingMove(Square, Square);
    template <Side::_t> void playPawnMove(Pi, Square, Square);
    template <Side::_t> void playCastling(Pi, Square, Square);

    template <Side::_t> void playMove(Square, Square);

public:
    constexpr Position () = default;
    Position (const Position&) = default;

    VectorPiMask myAlivePieces() const { return ps[My].alivePieces(); }
    Square mySquareOf(Pi pi) const { return ps[My].squareOf(pi); }

    Move createMove(Square from, Square to) const { return ps[My].createMove(from, to); }
    Score evaluate() const { return PositionSide::evaluate(ps[My], ps[Op]); }

    void playMove(const Position&, Square, Square);
    void playMove(Square, Square);

    //initial position setup
    bool drop(Side, PieceType, Square);
    bool afterDrop();
    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    bool setEnPassant(File);
};

#endif
