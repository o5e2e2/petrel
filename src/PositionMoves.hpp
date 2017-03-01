#ifndef POSITION_MOVES_HPP
#define POSITION_MOVES_HPP

#include "io.hpp"
#include "MatrixPiBb.hpp"
#include "Position.hpp"
#include "Zobrist.hpp"

class PositionSide;
class Position;
class Move;

class PositionMoves {
    //filled and used during move generation
    MatrixPiBb moves; //generated moves from side[My]

    Position pos;

    //legal move generation helpers
    template <Side::_t> void generateEnPassantMoves();
    template <Side::_t> void populateUnderpromotions();
    template <Side::_t> void generateCastlingMoves(Bb attackedSquares);
    template <Side::_t> void generateKingMoves(Bb attackedSquares);
    template <Side::_t> void generatePawnMoves();
    template <Side::_t> void excludePinnedMoves(VectorPiMask);
    template <Side::_t> void correctCheckEvasionsByPawns(Bb, Square);
    template <Side::_t> void generateCheckEvasions(Bb attackedSquares);

    template <Side::_t> void generateMoves();

public:
    void makeMove(const Position&, Square, Square, Zobrist = {});

    const PositionSide& side(Side) const;
    const Position& getPos() const { return pos; }

    const MatrixPiBb& getMoves() const { return moves; }
    MatrixPiBb& getMoves() { return moves; }

    MatrixPiBb cloneMoves() const { return MatrixPiBb{moves}; }

    bool is(Pi pi, Square to) const { return moves.is(pi, to); }

    Color setFen(std::istream&);
    Color makeMoves(std::istream&, Color);
    void limitMoves(std::istream&, Color);
};

#endif
