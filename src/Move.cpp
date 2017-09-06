#include "Move.hpp"

//convert internal move to long algebraic format
std::ostream& Move::write(std::ostream& out, Move move, Color colorToMove, ChessVariant chessVariant) {
    if (move.isNull()) { return out << "0000"; }

    Square moveFrom = move.from();
    Square moveTo = move.to();

    if (colorToMove.is(Black)) {
        moveFrom.flip();
        moveTo.flip();
    }

    if (!move.isSpecial()) {
        return out << moveFrom << moveTo;
    }

    if (move.from().is(Rank7)) {
        //the type of a promoted pawn piece encoded in place of to's rank
        Square promotedTo(File(moveTo), colorToMove.is(White)? Rank8 : Rank1);
        return out << moveFrom << promotedTo << move.promoType();
    }
    if (move.from().is(Rank5)) {
        //en passant capture move internally encoded as pawn captures pawn
        assert (move.to().is(Rank5));
        return out << moveFrom << Square{File{moveTo}, colorToMove.is(White)? Rank6 : Rank3};
    }
    if (move.from().is(Rank1)) {
        //castling move internally encoded as the rook captures the king
        switch (chessVariant) {
            case Orthodox:
                if (moveFrom.is(FileA)) {
                    return out << moveTo << Square{FileC, Rank{moveTo}};
                }
                else {
                    assert (moveFrom.is(FileH));
                    return out << moveTo << Square{FileG, Rank{moveTo}};
                }

            case Chess960:
                return out << moveTo << moveFrom;
        }
    }

    //should never happen
    return out << '?' << moveFrom << moveTo << '?';
}
