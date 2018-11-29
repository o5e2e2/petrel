#ifndef POSITION_MOVES_HPP
#define POSITION_MOVES_HPP

#include "MatrixPiBb.hpp"
#include "Position.hpp"
#include "Zobrist.hpp"

class PositionMoves : public Position {
protected:
    MatrixPiBb moves; //generated moves from side[My]
    Zobrist zobrist{0};
    index_t movesCount = 0;
    Score staticEval = Score::Draw;

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
    template <Side::_t> void generateMoves();

public:
    constexpr PositionMoves () = default;
    PositionMoves (const PositionMoves&) = default;

    void playMove(Square, Square);
    void playMove(const Position&, Square, Square, Zobrist);

    Zobrist generateZobrist() const;
    Zobrist createZobrist(Square, Square) const;
    void generateMoves();

    const MatrixPiBb& getMoves() const { return moves; }
    MatrixPiBb& getMoves() { return moves; }
    MatrixPiBb cloneMoves() const { return MatrixPiBb{moves}; }

    bool isLegalMove(Pi pi, Square to) const { return moves.is(pi, to); }
};

#endif
