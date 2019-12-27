#ifndef POSITION_HPP
#define POSITION_HPP

#include "PositionSide.hpp"

class Position {
    Side::array<PositionSide> ps;

    template <Side::_t> void updateSliderAttacks(VectorPiMask);
    template <Side::_t> void updateSliderAttacks(VectorPiMask, VectorPiMask);
    template <Side::_t> void setLegalEnPassant(Pi);
    template <Side::_t> void capture(Square);
    template <Side::_t> void playKingMove(Square, Square);
    template <Side::_t> void playPawnMove(Pi, Square, Square);
    template <Side::_t> void playCastling(Pi, Square, Square);
    template <Side::_t> void playMove(Square, Square);

protected:
    constexpr PositionSide& operator[] (Side side) { return (this->ps)[side]; }
    Side::array<Bb> occupiedBb; //all occupied squares by both sides, updated by combining both PositionSide::piecesBb

public:
    constexpr Position () = default;
    Position (const Position&) = default;

    constexpr const PositionSide& operator[] (Side side) const { return (this->ps)[side]; }

    VectorPiMask myAlivePieces() const { return (*this)[My].alivePieces(); }
    Square mySquareOf(Pi pi) const { return (*this)[My].squareOf(pi); }

    Move createMove(Square from, Square to) const { return (*this)[My].createMove(from, to); }
    Score evaluate() const { return PositionSide::evaluate((*this)[My], (*this)[Op]); }

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
