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

    friend std::ostream& operator << (std::ostream& out, const FenCastling& fenCastling) {
        if (fenCastling.castlingSet.empty()) {
            out << '-';
        }
        else {
            for (auto castling_symbol : fenCastling.castlingSet) { out << castling_symbol; }
        }
        return out;
    }

};

#endif
