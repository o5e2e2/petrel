#ifndef CASTLING_RULES_HPP
#define CASTLING_RULES_HPP

#include "Bb.hpp"
#include "BetweenSquares.hpp"

class CastlingRules {
    struct bb {
        Bb unimpeded;
        Bb unattacked;
    };

    File::static_array< File::static_array<bb> > castlingRules;

public:
    CastlingRules () {
        FOR_INDEX(File, kingFile) {
            FOR_INDEX(File, rookFile) {
                Square king(kingFile, Rank1);
                Square rook(rookFile, Rank1);

                if (king < rook) {
                    castlingRules[kingFile][rookFile].unimpeded  = ((::between(king, G1)+G1) | (::between(rook, F1)+F1)) % (Bb{king} + rook);
                    castlingRules[kingFile][rookFile].unattacked = (::between(king, G1)+G1) | king;
                }
                else if (rook < king) {
                    castlingRules[kingFile][rookFile].unimpeded  = ((::between(king, C1)+C1) | (::between(rook, D1)+D1)) % (Bb{king} + rook);
                    castlingRules[kingFile][rookFile].unattacked = (::between(king, C1)+C1) | king;
                }
                else {
                    castlingRules[kingFile][rookFile].unimpeded  = Bb(Rank1);
                    castlingRules[kingFile][rookFile].unattacked = Bb(Rank1);
                }
            }
        }
    }

    bool isLegal(Square king, Square rook, Bb occupied, Bb attacked) const {
        return (occupied & castlingRules[File(king)][File(rook)].unimpeded).none() && (attacked & castlingRules[File(king)][File(rook)].unattacked).none();
    }

    static CastlingSide castlingSide(Square king, Square rook) {
        assert (Rank(king) == Rank1);
        assert (Rank(rook) == Rank1);
        assert (king != rook);
        return (rook < king)? QueenSide:KingSide;
    }
};

extern const CastlingRules castlingRules;

#endif
