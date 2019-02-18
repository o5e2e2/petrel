#ifndef CASTLING_RULES_HPP
#define CASTLING_RULES_HPP

#include "Bb.hpp"
#include "BetweenSquares.hpp"

class CastlingRules {
    struct Rules {
        Bb unimpeded;
        Bb unattacked;
    };

    File::static_array< File::static_array<Rules> > castlingRules;

public:
    CastlingRules () {
        FOR_INDEX(File, kingFile) {
            FOR_INDEX(File, rookFile) {
                Square king(kingFile, Rank1);
                Square rook(rookFile, Rank1);

                if (king == rook) {
                    castlingRules[kingFile][rookFile].unimpeded  = Bb{};
                    castlingRules[kingFile][rookFile].unattacked = Bb{};
                    continue;
                }

                switch (CastlingRules::castlingSide(king, rook)) {
                    case QueenSide:
                        castlingRules[kingFile][rookFile].unimpeded  = ((::between(king, C1)+C1) | (::between(rook, D1)+D1)) % (Bb{king} + rook);
                        castlingRules[kingFile][rookFile].unattacked = (::between(king, C1)+C1) | king;
                        break;

                    case KingSide:
                        castlingRules[kingFile][rookFile].unimpeded  = ((::between(king, G1)+G1) | (::between(rook, F1)+F1)) % (Bb{king} + rook);
                        castlingRules[kingFile][rookFile].unattacked = (::between(king, G1)+G1) | king;
                        break;
                }
            }
        }
    }

    bool isLegal(Square king, Square rook, Bb occupied, Bb attacked) const {
        assert (king.is(Rank1));
        assert (rook.is(Rank1));
        assert (king != rook);
        return (occupied & castlingRules[File{king}][File{rook}].unimpeded).none() && (attacked & castlingRules[File{king}][File{rook}].unattacked).none();
    }

    static constexpr CastlingSide castlingSide(Square king, Square rook) {
        return (rook < king) ? QueenSide : KingSide;
    }

    static constexpr Square castlingKingTo(Square king, Square rook) {
        return castlingSide(king, rook).is(QueenSide) ? C1 : G1;
    }

    static constexpr Square castlingRookTo(Square king, Square rook) {
        return castlingSide(king, rook).is(QueenSide) ? D1 : F1;
    }

};

extern const CastlingRules castlingRules;

#endif
