#ifndef FEN_BOARD_HPP
#define FEN_BOARD_HPP

#include <set>
#include "io.hpp"
#include "typedefs.hpp"
#include "Square.hpp"

class Position;

/**
 * Setup a chess position from a FEN string with chess legality validation
 **/
class FenBoard {
    struct SquareImportance {
        bool operator () (Square sq1, Square sq2) const {
            if (Rank(sq1) != Rank(sq2)) {
                return Rank(sq1) < Rank(sq2); //Rank8 < Rank1
            }
            else {
                constexpr Rank::array<index_t> order{6, 4, 2, 0, 1, 3, 5, 7};
                return order[File(sq1)] < order[File(sq2)]; //FileD < FileE < FileC < FileF < FileB < FileG < FileA < FileH
            }
        }
    };
    typedef std::set<Square, SquareImportance> Squares;

    Color::array< PieceType::array<Squares> > pieces;
    Color::array<index_t> pieceCount = {{0, 0}};

    bool drop(Color, PieceType, Square);

public:
    friend io::istream& operator >> (io::istream&, FenBoard&);
    bool setPosition(Position& pos, Color colorToMove);
};

#endif
