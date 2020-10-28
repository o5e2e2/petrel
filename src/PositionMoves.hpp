#ifndef POSITION_MOVES_HPP
#define POSITION_MOVES_HPP

#include "Position.hpp"
#include "MatrixPiBb.hpp"
#include "Zobrist.hpp"

class PositionMoves : public Position {
    MatrixPiBb moves; //generated moves from My side
    Bb attackedSquares; //squares attacked by all opponent pieces

    Zobrist zobrist{0};
    Score staticEval = Score::None;
    index_t movesCount = 0;
    bool inCheck = false;

private:
    //legal move generation helpers
    template <Side::_t> void excludePinnedMoves(VectorPiMask);
    template <Side::_t> void correctCheckEvasionsByPawns(Bb, Square);
    template <Side::_t> void populateUnderpromotions();
    template <Side::_t> void generateEnPassantMoves();
    template <Side::_t> void generatePawnMoves();
    template <Side::_t> void generateCastlingMoves();
    template <Side::_t> void generateLegalKingMoves();
    template <Side::_t> void generateCheckEvasions();
    template <Side::_t> void generateMoves();

    Zobrist createZobrist(Square, Square) const;
    Zobrist generateZobrist() const;

protected:
    void playMove(Square, Square);
    void playMove(const Position&, Square, Square, Zobrist);

    void generateMoves();

    void clearMove(Pi pi, Square sq) { moves.clear(pi, sq); }
    void setMoves(const MatrixPiBb& m) { moves = m; }

    void setZobrist(const PositionMoves& parent, Square from, Square to) { zobrist = parent.createZobrist(from, to); }
    void setZobrist() { zobrist = generateZobrist(); }

public:
    constexpr PositionMoves () = default;
    PositionMoves (const PositionMoves&) = default;

    const Zobrist& getZobrist() const { return zobrist; }
    index_t getMovesCount() const { return movesCount; }
    Score getStaticEval() const { return staticEval; }

    MatrixPiBb cloneMoves() const { return MatrixPiBb{moves}; }

    bool isLegalMove(Square from, Square to) const;
};

#endif
