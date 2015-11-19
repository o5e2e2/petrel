#ifndef POSITION_HPP
#define POSITION_HPP

#include "io.hpp"
#include "PositionSide.hpp"
#include "DualBbOccupied.hpp"
#include "Move.hpp"
#include "PositionFen.hpp"

class Position {
    friend class PositionMoves;

    Side::array<PositionSide> side;
    DualBbOccupied occupied; //pieces of both sides
    Zobrist zobrist;

private:
    void set(Side, Pi, PieceType, Square);

    template <Side::_t> const Bb& pinRayFrom(Pi) const;
    template <Side::_t> bool isLegalEnPassant(Pi, File) const;
    bool isLegalEnPassantBefore(Pi, Square) const;
    template <Side::_t> void setLegalEnPassant(Pi);

    template <Side::_t> void move(Pi, Square, Square);
    template <Side::_t> void movePawn(Pi, Square, Square);
    template <Side::_t> void updateSliderAttacks(VectorPiMask);
    template <Side::_t> void updateSliderAttacksKing(VectorPiMask); //remove king to avoid hiding it under its own shadow when in check

    template <Side::_t> void makeKingMove(Square, Square);
    template <Side::_t> void makePawnMove(Pi, Square, Square);
    template <Side::_t> void makeCastling(Pi, Square, Square);
    template <Side::_t> void makeMove(Square, Square);

    void syncSides();

public:
    Position () {};
    Position (int) : side() {};

    friend void PositionFen::write(std::ostream&, const Position&, Color, ChessVariant);
    friend Move::Move(const Position&, Square, Square);
    friend Move::Move(std::istream& in, const Position&, Color);

    Zobrist getZobrist() const;

    void makeMove(const Position& parent, Square from, Square to);

    void makeMove(Zobrist z, const Position& parent, Square from, Square to) {
        zobrist = z; makeMove(parent, from, to);
    }

    Zobrist makeZobrist(Square from, Square to) const;

    //initial position setup
    bool drop(Side, PieceType, Square);
    bool setup();
    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    bool setEnPassant(File);
    void setZobrist();
};

#endif
