#ifndef FEN_CASTLING_HPP
#define FEN_CASTLING_HPP

#include <set>
#include "io.hpp"

class PositionSide;

class FenCastling {
    std::set<io::char_type> castlingSet;

    void insert(const PositionSide&, Color, ChessVariant);

public:
    FenCastling (const PositionSide& white, const PositionSide& black, ChessVariant chessVariant) {
        insert(white, White, chessVariant);
        insert(black, Black, chessVariant);
    }

    friend std::ostream& operator << (std::ostream& out, const FenCastling&);

};

#endif
