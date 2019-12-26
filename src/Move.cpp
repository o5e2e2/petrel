#include "Move.hpp"
#include "PositionSide.hpp"

//convert internal move to long algebraic format
io::ostream& Move::write(io::ostream& out, Move move, Color colorToMove, ChessVariant chessVariant) {
    if (!move) { return out << "0000"; }

    Square moveFrom = move.from();
    Square moveTo = move.to();

    if (colorToMove.is(Black)) {
        moveFrom.flip();
        moveTo.flip();
    }

    if (!move.isSpecial()) {
        return out << moveFrom << moveTo;
    }

    if (move.from().on(Rank7)) {
        //the type of a promoted pawn piece encoded in place of to's rank
        Square promotedTo(File(moveTo), colorToMove.is(White) ? Rank8 : Rank1);
        return out << moveFrom << promotedTo << move.promoType();
    }
    if (move.from().on(Rank5)) {
        //en passant capture move internally encoded as pawn captures pawn
        assert (move.to().on(Rank5));
        return out << moveFrom << Square{File(moveTo), colorToMove.is(White) ? Rank6 : Rank3};
    }
    if (move.from().on(Rank1)) {
        //castling move internally encoded as the rook captures the king
        switch (chessVariant) {
            case Chess960:
                return out << moveTo << moveFrom;

            case Orthodox:
                if (moveFrom.on(FileA)) {
                    return out << moveTo << Square{FileC, Rank(moveTo)};
                }
                if (moveFrom.on(FileH)) {
                    return out << moveTo << Square{FileG, Rank(moveTo)};
                }
        }
    }

    //should never happen
    return out << '?' << moveFrom << moveTo << '?';
}
