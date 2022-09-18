#ifndef POSITION_HPP
#define POSITION_HPP

#include "PositionSide.hpp"
#include "Score.hpp"

class Position {
    Side::arrayOf<PositionSide> positionSide;

    template <Side::_t> void updateSliderAttacks(PiMask);
    template <Side::_t> void updateSliderAttacks(PiMask, PiMask);
    template <Side::_t> void capture(Square);
    template <Side::_t> void playKingMove(Square, Square);
    template <Side::_t> void playPawnMove(Pi, Square, Square);
    template <Side::_t> void playCastling(Pi, Square, Square);
    template <Side::_t> void playMove(Square, Square);

protected:
    constexpr PositionSide& operator[] (Side side) { return positionSide[side]; }

public:
    constexpr Position () = default;
    Position (const Position&) = default;

    constexpr const PositionSide& operator[] (Side side) const { return positionSide[side]; }

    Move createMove(Square from, Square to) const { return (*this)[My].createMove(from, to); }
    Score evaluate() const { return PositionSide::evaluate((*this)[My], (*this)[Op]); }

    void playMove(const Position&, Square, Square);
    void playMove(Square, Square);

    //initial position setup
    bool dropValid(Side, PieceType, Square);
    bool afterDrop();
    template <Side::_t> void setLegalEnPassant(Pi);
};

#endif
