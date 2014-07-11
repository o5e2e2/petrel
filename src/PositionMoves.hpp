#ifndef POSITION_MOVES_HPP
#define POSITION_MOVES_HPP

#include "MatrixPiBb.hpp"

class PositionSide;
class Position;
class Node;

class PositionMoves {
    const Position& pos;

    //filled and used during move generation
    MatrixPiBb moves; //generated moves from side[My]
    Bb attacked; //squares attacked by side[Op]

    PositionMoves () = delete;
    PositionMoves& operator = (const PositionMoves&) = delete;

    //legal move generation helpers
    template <Side::_t> void generateEnPassantMoves();
    template <Side::_t> void generateUnderpromotions();
    template <Side::_t> void generateKingMoves();
    template <Side::_t> void excludePinnedMoves(VectorPiMask);
    template <Side::_t> void generatePawnCheckEvasions(Square, Bb);
    template <Side::_t> void generateCheckEvasions();
    template <Side::_t> void generateMoves();

    void swapSides();

public:
    PositionMoves (const Position& p);

    const PositionSide& side(Side) const;
    index_t countLegalMoves() const;
    void limitMoves(std::istream&, MatrixPiBb&, Color) const;
    MatrixPiBb& getMoves() { return moves; }
    const Position& getPos() const { return pos;}

    Color makeMoves(std::istream&, Color);
    bool eval(Node&);
};

#endif
