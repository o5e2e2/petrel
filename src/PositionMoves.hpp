#ifndef POSITION_MOVES_HPP
#define POSITION_MOVES_HPP

#include "io.hpp"
#include "MatrixPiBb.hpp"
#include "Position.hpp"
#include "Zobrist.hpp"

class PositionSide;
class Move;

class PositionMoves {
    Position pos;
    MatrixPiBb moves; //generated moves from side[My]
    const PositionMoves& parent;

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
    PositionMoves (int) : pos(0), moves(0), parent(*this) {}
    PositionMoves (const PositionMoves& p, int) : pos(0), moves(0), parent(p) {}
    PositionMoves (const PositionMoves&) = default;

    void makeMove(Square, Square, Zobrist = {});
    Zobrist makeZobrist(Square from, Square to) const { return parent.pos.makeZobrist(from, to); }

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
