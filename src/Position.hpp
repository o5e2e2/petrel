#ifndef POSITION_HPP
#define POSITION_HPP

#include "PositionSide.hpp"
#include "Score.hpp"

class Position {
    Side::array<PositionSide> ps;

    template <Side::_t> void updateSliderAttacks(VectorPiMask);
    template <Side::_t> void updateSliderAttacks(VectorPiMask, VectorPiMask);
    template <Side::_t> void capture(Square);
    template <Side::_t> void playKingMove(Square, Square);
    template <Side::_t> void playPawnMove(Pi, Square, Square);
    template <Side::_t> void playCastling(Pi, Square, Square);
    template <Side::_t> void playMove(Square, Square);

protected:
    Side::array<Bb> occupiedBb; //all occupied squares by both sides, updated by combining both PositionSide::piecesBb
    constexpr PositionSide& operator[] (Side side) { return (this->ps)[side]; }

public:
    constexpr Position () = default;
    Position (const Position&) = default;

    constexpr const PositionSide& operator[] (Side side) const { return (this->ps)[side]; }

    Move createMove(Square from, Square to) const { return (*this)[My].createMove(from, to); }
    Score evaluate() const { return PositionSide::evaluate((*this)[My], (*this)[Op]); }

    void playMove(const Position&, Square, Square);
    void playMove(Square, Square);

    //initial position setup
    bool drop(Side, PieceType, Square);
    bool afterDrop();
    template <Side::_t> void setLegalEnPassant(Pi);
};

#endif
