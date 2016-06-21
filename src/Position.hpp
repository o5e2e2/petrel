#ifndef POSITION_HPP
#define POSITION_HPP

#include "io.hpp"
#include "DualBbOccupied.hpp"
#include "Move.hpp"
#include "PositionSide.hpp"
#include "Zobrist.hpp"

class FenBoard;

class Position {
    friend class PositionMoves;

    Side::array<PositionSide> side;
    DualBbOccupied occupied; //pieces of both sides
    Zobrist zobrist;

private:
    void set(Side, Pi, PieceType, Square);

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

    bool drop(Side, PieceType, Square);
    bool setup();
    bool setBoard(FenBoard&, Color);
    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    bool setEnPassant(File);

    Color setBoard(std::istream&);
    std::istream& setCastling(std::istream&, Color);
    std::istream& setEnPassant(std::istream&, Color);

    static void fenBoard(std::ostream&, const PositionSide& white, const PositionSide& black);
    void fenEnPassant(std::ostream& out, Color colorToMove) const;

public:
    Position () {};
    Position (int) : side() {};

    Zobrist getZobrist() const;

    void makeMove(const Position&, Square, Square);

    void makeMove(Zobrist z, const Position& parent, Square from, Square to) {
        zobrist = z;
        makeMove(parent, from, to);
    }

    Zobrist makeZobrist(Square from, Square to) const;

    //initial position setup
    Color setStartpos();
    Color setFen(std::istream&);
    Color makeMoves(std::istream&, Color);

    void fen(std::ostream&, Color, ChessVariant) const;
    Move operator() (Square, Square) const;
    Move operator() (std::istream& in, Color) const;

};

#endif
