#ifndef POSITION_MOVES_HPP
#define POSITION_MOVES_HPP

#include "io.hpp"
#include "MatrixPiBb.hpp"

class PositionSide;
class Position;

class PositionMoves {
    //filled and used during move generation
    MatrixPiBb moves; //generated moves from side[My]

    const Position& pos;

    PositionMoves () = delete;
    PositionMoves& operator = (const PositionMoves&) = delete;

    //legal move generation helpers
    template <Side::_t> void generateEnPassantMoves();
    template <Side::_t> void generateUnderpromotions();
    template <Side::_t> void generateCastlingMoves(Bb attackedSquares);
    template <Side::_t> void generateKingMoves(Bb attackedSquares);
    template <Side::_t> void generatePawnMoves();
    template <Side::_t> void excludePinnedMoves(VectorPiMask);
    template <Side::_t> void correctCheckEvasionsByPawns(Bb, Square);
    template <Side::_t> void generateCheckEvasions(Bb attackedSquares);

    template <Side::_t> void generateMoves();

public:
    PositionMoves (const Position& p) : pos(p) { generateMoves<My>(); }

    const PositionSide& side(Side) const;
    MatrixPiBb& getMoves() { return moves; }
    const Position& getPos() const { return pos;}

    void limitMoves(std::istream&, MatrixPiBb&, Color) const;
    void makeMoves(std::istream&, Color&);
};

#endif
