#include "Move.hpp"

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
