#ifndef POSITION_FEN_HPP
#define POSITION_FEN_HPP

#include <set>
#include "io.hpp"
#include "Square.hpp"

class Position;

/**
 * Setup a chess position from a FEN string with partial chess legality validation
 **/
class PositionFen {
    Position& pos;

    struct SquareOrder { bool operator () (Square, Square) const; };
    typedef std::set<Square, SquareOrder> Squares;
    typedef Color::array< PieceType::array<Squares> > ColorTypeSquares;

    std::istream& readBoard(std::istream&, ColorTypeSquares&);
    bool drop(ColorTypeSquares&, Color, PieceType, Square);
    bool setupBoard(ColorTypeSquares&, Color);

    std::istream& readCastling(std::istream&, Color);
    std::istream& readEnPassant(std::istream&, Color);
    void readNumbers(std::istream&);

public:
    PositionFen (Position& p): pos(p) {};
    Color readFen(std::istream&);
};

#endif
