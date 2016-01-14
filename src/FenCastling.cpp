#include "FenCastling.hpp"
#include "CastlingRules.hpp"
#include "Position.hpp"
#include "PositionSide.hpp"

void FenCastling::insert(const PositionSide& side, Color color, ChessVariant chessVariant) {
    for (Pi pi : side.castlingRooks()) {
        io::char_type castling_symbol;

        switch (chessVariant) {
            case Orthodox:
                castling_symbol = CastlingRules::castlingSide(side.kingSquare(), side.squareOf(pi)).to_char();
                break;

            case Chess960:
                castling_symbol = File{side.squareOf(pi)}.to_char();
                break;
        }

        if (color.is(White)) {
            castling_symbol = static_cast<io::char_type>(std::toupper(castling_symbol));
        }

        castlingSet.insert(castling_symbol);
    }
}

std::istream& FenCastling::read(std::istream& in, Position& pos, Color colorToMove) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    for (io::char_type c; in.get(c) && !std::isblank(c); ) {
        if (std::isalpha(c)) {
            Color color(std::isupper(c)? White : Black);
            Side si(color.is(colorToMove)? My : Op);

            c = static_cast<io::char_type>(std::tolower(c));

            CastlingSide castlingSide{CastlingSide::Begin};
            if ( castlingSide.from_char(c) ) {
                if (pos.setCastling(si, castlingSide)) {
                    continue;
                }
            }
            else {
                File file{File::Begin};
                if ( file.from_char(c) ) {
                    if (pos.setCastling(si, file)) {
                        continue;
                    }
                }
            }
        }
        io::fail_char(in);
    }
    return in;
}
