#include "Move.hpp"
#include "Position.hpp"

#define MY pos.side[My]
#define OP pos.side[Op]
#define OCCUPIED pos.occupied[My]

Move::Move(const Position& pos, Square move_from, Square move_to) {
    if ( MY.isPawn(MY.pieceOn(move_from)) ) {
        if (move_from.is(Rank7)) {
            *this = Move::special(move_from, move_to);
            return;
        }
        if (move_from.is(Rank5) && move_to.is(Rank5)) {
            *this = Move::special(move_from, move_to);
            return;
        }
    }
    else if (MY.kingSquare().is(move_to)) {
        *this = Move::special(move_from, move_to);
        return;
    }

    *this = Move(move_from, move_to);
}

Move::Move(std::istream& in, const Position& pos, Color colorToMove) {
    auto before_move = in.tellg();

    Square move_from{Square::Begin};
    Square move_to{Square::Begin};
    if (in >> std::ws >> move_from >> move_to) {
        if (colorToMove.is(Black)) {
            move_from.flip();
            move_to.flip();
        }

        if (MY.isPieceOn(move_from)) {
            Pi pi{MY.pieceOn(move_from)};

            //convert special moves (castling, promotion, ep) to the internal move format
            if (MY.isPawn(pi)) {
                if (move_from.is(Rank7)) {
                    PromoType promo{Queen};
                    in >> promo;
                    in.clear(); //promotion piece is optional
                    *this = Move::promotion(move_from, move_to, promo);
                    return;
                }
                if (move_from.is(Rank5) && OP.hasEnPassant() && OP.enPassantFile().is(File(move_to))) {
                    *this = Move::enPassant(move_from, Square{File(move_to), Rank5});
                    return;
                }
            }
            else if (pi.is(TheKing)) {
                if (MY.isPieceOn(move_to)) { //Chess960 castling encoding
                    *this = Move::castling(move_to, move_from);
                    return;
                }
                if (move_from.is(E1) && move_to.is(G1) && MY.isPieceOn(H1)) {
                    *this = Move::castling(H1, E1);
                    return;
                }
                if (move_from.is(E1) && move_to.is(C1) && MY.isPieceOn(A1)) {
                    *this = Move::castling(A1, E1);
                    return;
                }
            }

            *this = Move(move_from, move_to);
            return;
        }
    }

    io::fail_pos(in, before_move);
    *this = Move{};
}

//convert internal move to long algebraic format
std::ostream& Move::write(std::ostream& out, Move move, Color colorToMove, ChessVariant chessVariant) {
    if (move.isNull()) { return out << "0000"; }

    Square move_from = move.from();
    Square move_to = move.to();

    if (colorToMove.is(Black)) {
        move_from.flip();
        move_to.flip();
    }

    if (!move.isSpecial()) {
        return out << move_from << move_to;
    }
    if (move.from().is(Rank7)) {
        //the type of a promoted pawn piece encoded in place of to's rank
        PromoType promo = Move::decodePromoType(move.to());

        Square promoted_to(File(move_to), colorToMove.is(White)? Rank8 : Rank1);
        return out << move_from << promoted_to << promo;
    }
    if (move.from().is(Rank5)) {
        //en passant capture move internally encoded as pawn captures pawn
        assert (move.to().is(Rank5));
        return out << move_from << Square{File{move_to}, colorToMove.is(White)? Rank6 : Rank3};
    }
    if (move.from().is(Rank1)) {
    //castling move internally encoded as the rook captures the king
        switch (chessVariant) {
            case Orthodox:
                if (move_from.is(FileA)) {
                    return out << move_to << Square{FileC, Rank{move_to}};
                }
                else {
                    assert (move_from.is(FileH));
                    return out << move_to << Square{FileG, Rank{move_to}};
                }

            case Chess960:
                return out << move_to << move_from;
        }
    }

    //should never happen
    return out << '?' << move_from << move_to << '?';
}

#undef MY
#undef OP
#undef OCCUPIED


