#ifndef POSITION_MOVES_HPP
#define POSITION_MOVES_HPP

#include "MatrixPiBb.hpp"
#include "Position.hpp"
#include "Zobrist.hpp"

class PositionMoves : public Position {
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

    Zobrist createZobrist(Square, Square) const;

protected:
    void playMove(Square, Square);
    void playMove(const Position&, Square, Square, Zobrist);

    void setMoves();

    void clearMove(Pi pi, Square sq) { moves.clear(pi, sq); }
    void setMoves(const MatrixPiBb& m) { moves = m; }

    void setZobrist(const PositionMoves& p, Square from, Square to) { zobrist = p.createZobrist(from, to); }
    void setZobrist() { zobrist = generateZobrist(); }

public:
    constexpr PositionMoves () = default;
    PositionMoves (const PositionMoves&) = default;

    Zobrist generateZobrist() const;

    const Zobrist& getZobrist() const { return zobrist; }
    index_t getMovesCount() const { return movesCount; }
    Score getStaticEval() const { return staticEval; }

    const MatrixPiBb& getMoves() const { return moves; }
    MatrixPiBb cloneMoves() const { return MatrixPiBb{moves}; }

    bool isLegalMove(Pi pi, Square to) const { return moves.is(pi, to); }
};

#endif
