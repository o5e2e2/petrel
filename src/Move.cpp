#include "Move.hpp"

//convert internal move to long algebraic format
std::ostream& Move::write(std::ostream& out, Move move, Color colorToMove, ChessVariant chessVariant) {
    if (move.isNull()) { return out << "0000"; }

    Square move_from = (colorToMove == White)? move.from():~move.from();
    Square move_to = (colorToMove == White)? move.to():~move.to();

    if (!move.isSpecial()) {
        out << move_from << move_to;
    }
    else {
        if (move.from().is(Rank7)) {
            //the type of a promoted pawn piece encoded in place of to's rank
            Square promoted_to(File(move_to), (colorToMove == White)? Rank8:Rank1);
            PromoType promo = Move::decodePromoType(move.to());
            out << move_from << promoted_to << promo;
        }
        else if (move.from().is(Rank1)) {
            //castling move internally encoded as the rook captures the king
            if (chessVariant == Chess960) {
                out << move_to << move_from;
            }
            else {
                if (move_from.is(FileA)) {
                    out << move_to << Square{FileC, Rank{move_from}};
                }
                else {
                    assert (move_from.is(FileH));
                    out << move_to << Square{FileG, Rank{move_from}};
                }
            }
        }
        else {
            //en passant capture move internally encoded as pawn captures pawn
            assert (move.from().is(Rank5));
            assert (move.to().is(Rank5));
            out << move_from << Square{File{move_to}, (colorToMove == White)? Rank6:Rank3};
        }
    }
    return out;
}


