#ifndef CASTLING_RULES_HPP
#define CASTLING_RULES_HPP

#include "Bb.hpp"
#include "BetweenSquares.hpp"

class CastlingRules {
    CastlingSide::static_array<Bb> unimpeded;
    CastlingSide::array<Bb> unattacked;

public:
    CastlingRules (Square king = E1, Square kingSideRook = H1, Square queenSideRook = A1) {
        set(king, kingSideRook);
        set(king, queenSideRook);
    }

    void set(Square king, Square rook) {
        assert (king.is<Rank1>() && rook.is<Rank1>() && king != rook);
        if (king < rook) {
            unimpeded[KingSide]  = ((::between(king, G1)+G1) | (::between(rook, F1)+F1)) % (Bb{king} | rook);
            unattacked[KingSide]  = (::between(king, G1)+G1) | king;
        }
        else {
            unimpeded[QueenSide] = ((::between(king, C1)+C1) | (::between(rook, D1)+D1)) % (Bb{king} | rook);
            unattacked[QueenSide] = (::between(king, C1)+C1) | king;
        }
    }

    bool isLegal(CastlingSide side, Bb occupied, Bb attacked) const {
        return (occupied & unimpeded[side]).none() && (attacked & unattacked[side]).none();
    }
};

extern CastlingRules castlingRules; //constant during the same Chess960 game

#endif
