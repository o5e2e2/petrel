#ifndef POSITION_MOVES_HPP
#define POSITION_MOVES_HPP

#include "MatrixPiBb.hpp"
#include "Position.hpp"
#include "Zobrist.hpp"

class PositionMoves : public Position {
protected:
    MatrixPiBb moves; //generated moves from side[My]
    template <Side::_t> void generateMoves();

private:
    //legal move generation helpers
    template <Side::_t> void excludePinnedMoves(VectorPiMask);
    template <Side::_t> void correctCheckEvasionsByPawns(Bb, Square);
    template <Side::_t> void populateUnderpromotions();
    template <Side::_t> void generateEnPassantMoves();
    template <Side::_t> void generatePawnMoves();
    template <Side::_t> void generateCastlingMoves(Bb attackedSquares);
    template <Side::_t> void generateKingMoves(Bb attackedSquares);
    template <Side::_t> void generateCheckEvasions(Bb attackedSquares);

public:
    PositionMoves (int) : Position(0), moves(0) {}
    PositionMoves (const PositionMoves&) = default;

    void playMove(const Position&, Square, Square, Zobrist = {});

    const MatrixPiBb& getMoves() const { return moves; }
    MatrixPiBb& getMoves() { return moves; }
    MatrixPiBb cloneMoves() const { return MatrixPiBb{moves}; }

    bool is(Pi pi, Square to) const { return moves.is(pi, to); }
};

#endif
