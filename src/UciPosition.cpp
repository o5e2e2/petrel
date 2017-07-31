#include "UciPosition.hpp"
#include <set>
#include "CastlingRules.hpp"

#define MY side[My]
#define OP side[Op]
#define OCCUPIED side[My].occupied()

Move UciPosition::readMove(std::istream& in) const {
    auto before = in.tellg();

    Square moveFrom{Square::Begin};
    Square moveTo{Square::Begin};
    if (in >> std::ws >> moveFrom >> moveTo) {
        if (colorToMove.is(Black)) {
            moveFrom.flip();
            moveTo.flip();
        }

        if (MY.isPieceOn(moveFrom)) {
            Pi pi{MY.pieceOn(moveFrom)};

            //convert special moves (castling, promotion, ep) to the internal move format
            if (MY.isPawn(pi)) {
                if (moveFrom.is(Rank7)) {
                    PromoType promo{Queen};
                    in >> promo;
                    in.clear(); //promotion piece is optional
                    return Move::promotion(moveFrom, moveTo, promo);
                }
                if (moveFrom.is(Rank5) && OP.hasEnPassant() && OP.enPassantFile().is(File(moveTo))) {
                    return Move::enPassant(moveFrom, Square{File(moveTo), Rank5});
                }
            }
            else if (pi.is(TheKing)) {
                if (MY.isPieceOn(moveTo)) { //Chess960 castling encoding
                    return Move::castling(moveTo, moveFrom);
                }
                if (moveFrom.is(E1) && moveTo.is(G1) && MY.isPieceOn(H1)) {
                    return Move::castling(H1, E1);
                }
                if (moveFrom.is(E1) && moveTo.is(C1) && MY.isPieceOn(A1)) {
                    return Move::castling(A1, E1);
                }
            }

            return Move(moveFrom, moveTo);
        }
    }

    io::fail_pos(in, before);
    return Move{};
}

void UciPosition::limitMoves(std::istream& in) {
    MatrixPiBb searchMoves;
    index_t limit = 0;

    while (in) {
        auto before = in.tellg();

        Move m = readMove(in);
        if (in) {
            Square from{ m.from() };
            Square to{ m.to() } ;
            Pi pi{ MY.pieceOn(from) };

            if (moves.is(pi, to) && !searchMoves.is(pi, to)) {
                searchMoves.set(pi, to);
                ++limit;
                continue;
            }

        }

        io::fail_pos(in, before);
    }

    if (limit > 0) {
        moves = searchMoves;
        in.clear();
        return;
    }

    if (in.eof()) {
        io::fail_rewind(in);
    }
}

std::istream& operator >> (std::istream& in, UciPosition& pos) {
    pos.colorToMove = pos.setupFromFen(in);
    pos.generateMoves<My>();

    if (in) {
        unsigned _fifty;
        unsigned _moves;
        in >> _fifty >> _moves;
        in.clear(); //ignore missing optional 'fifty' and 'moves' fen fields
    }

    return in;
}

void UciPosition::playMoves(std::istream& in) {
    while (in) {
        auto before = in.tellg();

        Move m = readMove(in);
        if (in) {
            Square from{ m.from() };
            Square to{ m.to() } ;
            Pi pi{ side[My].pieceOn(from) };

            if (is(pi, to)) {
                Position::playMove(from, to);
                generateMoves<My>();
                colorToMove.flip();
                continue;
            }
        }

        io::fail_pos(in, before);
    }
}

void UciPosition::fenEnPassant(std::ostream& out) const {
    if (!OP.hasEnPassant()) {
        out << '-';
        return;
    }

    out << Square(OP.enPassantFile(), colorToMove.is(White)? Rank6 : Rank3);
}

class WriteFenBoard {
    const PositionSide& whiteSide;
    const PositionSide& blackSide;

public:
    WriteFenBoard (const UciPosition& pos) :
        whiteSide(pos.side[pos.colorToMove.is(White)? My : Op]),
        blackSide(pos.side[pos.colorToMove.is(Black)? My : Op])
        {}

    friend std::ostream& operator << (std::ostream& out, const WriteFenBoard& fen) {
        FOR_INDEX(Rank, rank) {
            index_t blank_squares = 0;

            FOR_INDEX(File, file) {
                Square sq(file,rank);

                if (fen.whiteSide.isOccupied(sq)) {
                    if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                    out << static_cast<io::char_type>(std::toupper( fen.whiteSide.typeOf(sq).to_char() ));
                    continue;
                }

                if (fen.blackSide.isOccupied(~sq)) {
                    if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                    out << fen.blackSide.typeOf(~sq).to_char();
                    continue;
                }

                ++blank_squares;
            }

            if (blank_squares != 0) { out << blank_squares; }
            if (rank != Rank1) { out << '/'; }
        }

        return out;
    }
};

class WriteFenCastling {
    std::set<io::char_type> castlingSet;

    void insert(const PositionSide& side, Color color, ChessVariant chessVariant) {
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

public:
    WriteFenCastling (const UciPosition& pos) {
        insert(pos.side[pos.colorToMove.is(White)? My : Op], White, pos.chessVariant);
        insert(pos.side[pos.colorToMove.is(Black)? My : Op], Black, pos.chessVariant);
    }

    friend std::ostream& operator << (std::ostream& out, const WriteFenCastling& fen) {
        if (fen.castlingSet.empty()) {
            return out << '-';
        }

        for (auto castling_symbol : fen.castlingSet) {
            out << castling_symbol;
        }

        return out;
    }
};

std::ostream& operator << (std::ostream& out, const UciPosition& pos) {
    out << WriteFenBoard(pos)
        << ' '
        << pos.colorToMove
        << ' '
        << WriteFenCastling(pos)
        << ' ';
    pos.fenEnPassant(out);
    return out;
}

#undef MY
#undef OP
#undef OCCUPIED
