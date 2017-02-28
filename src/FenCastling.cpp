#include "FenCastling.hpp"
#include "CastlingRules.hpp"
#include "Position.hpp"
#include "PositionSide.hpp"

void FenCastling::insert(const PositionSide& side, Color color, ChessVariant chessVariant) {
    for (Pi pi : side.castlingRooks()) {
        io::char_type castling_symbol;

        switch (chessVariant) {
            case Chess960:
                castling_symbol = File{side.squareOf(pi)}.to_char();
                break;

            case Orthodox:
            default:
                castling_symbol = CastlingRules::castlingSide(side.kingSquare(), side.squareOf(pi)).to_char();
                break;
        }

        if (color.is(White)) {
            castling_symbol = static_cast<io::char_type>(std::toupper(castling_symbol));
        }

        castlingSet.insert(castling_symbol);
    }
}

std::ostream& operator << (std::ostream& out, const FenCastling& fenCastling) {
    if (fenCastling.castlingSet.empty()) {
        return out << '-';
    }

    for (auto castling_symbol : fenCastling.castlingSet) {
        out << castling_symbol;
    }

    return out;
}
