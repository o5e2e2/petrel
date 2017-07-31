#include "UciPosition.hpp"

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

void UciPosition::setFen(std::istream& in) {
    colorToMove = Position::setFen(in);
    generateMoves<My>();
}

void UciPosition::makeMoves(std::istream& in) {
    while (in) {
        auto before = in.tellg();

        Move m = readMove(in);
        if (in) {
            Square from{ m.from() };
            Square to{ m.to() } ;
            Pi pi{ side[My].pieceOn(from) };

            if (is(pi, to)) {
                Position::makeMove(from, to);
                generateMoves<My>();
                colorToMove.flip();
                continue;
            }
        }

        io::fail_pos(in, before);
    }
}

#undef MY
#undef OP
#undef OCCUPIED
