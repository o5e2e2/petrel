#ifndef POSITION_HPP
#define POSITION_HPP

#include "io.hpp"
#include "PositionSide.hpp"
#include "Move.hpp"
#include "PositionFen.hpp"

class Position {
    friend class PositionMoves;

    Side::array<PositionSide> side;
    Side::array<Bb> occupied; //pieces of both sides
    Zobrist zobrist;

private:
    Position& operator = (const Position&) = delete;
    Position (const Position&) = delete;

    void set(Side, Pi, PieceType, Square);

    template <Side::_t> void move(Pi, Square, Square);
    template <Side::_t> void updateSliderAttacks(VectorPiMask);
    template <Side::_t> void updateSliderAttacksKing(VectorPiMask); //remove king to avoid hiding it under its own shadow when in check

    template <Side::_t> void makeKingMove(Square, Square);
    template <Side::_t> void makePawnMove(Pi, Square, Square);
    template <Side::_t> void makeCastling(Pi, Square, Square);

    template <Side::_t> const Bb& pinLineFrom(Pi) const;
    template <Side::_t> bool isLegalEnPassant(Pi, Square) const;
    template <Side::_t> void setLegalEnPassant(Pi);
    template <Side::_t> void clearEnPassant();

    void swapSides();
    void syncSides();

public:
    Position ();
    Position (const Position& parent, int); //null move
    Position (const Position& parent, Square from, Square to);
    Position& operator = (Position&&) = default;

    friend void PositionFen::write(std::ostream&, const Position&, Color, ChessVariant);
    Move createMove(Side, Square, Square) const;

    //serie of irreversible moves with extra legality check
    friend Move readMove(std::istream&, const Position&, Color);

    template <Side::_t> void makeMove(Square, Square);

    //initial position setup
    bool drop(Side, PieceType, Square);
    bool setup();
    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    bool setEnPassant(Square);
};

#endif
