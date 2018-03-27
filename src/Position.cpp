#include "Position.hpp"
#include "BetweenSquares.hpp"
#include "CastlingRules.hpp"
#include "PieceTypeAttack.hpp"

#define MY side[My]
#define OP side[Op]
#define OCCUPIED side[My].occupied()

void Position::clear() {
    MY.clear();
    OP.clear();
}

bool Position::setup() {
    MY.setOpKing(~OP.kingSquare());
    OP.setOpKing(~MY.kingSquare());
    updateSliderAttacks<Op>(OP.alivePieces(), MY.alivePieces());
    setGamePhase<My>();
    setGamePhase<Op>();
    return MY.attacksToKing().none(); //not in check
}

bool Position::drop(Side My, PieceType ty, Square to) {
    return MY.drop(ty, to);
}

bool Position::setCastling(Side My, File file) {
    return MY.setCastling(file);
}

bool Position::setCastling(Side My, CastlingSide castlingSide) {
    return MY.setCastling(castlingSide);
}

template <Side::_t My>
void Position::updateSliderAttacks(VectorPiMask affected) {
    constexpr Side Op{~My};

    //sync occupiedBb between sides
    MY.setOccupied(MY.occupiedSquares() + ~OP.occupiedSquares());
    OP.setOccupied(OP.occupiedSquares() + ~MY.occupiedSquares());

    //TRICK: attacks calculated without opponent's king for implicit out of check king's moves generation
    MY.setSliderAttacks(affected & MY.sliders(), OCCUPIED - MY.opKingSquare());
}

template <Side::_t My>
void Position::updateSliderAttacks(VectorPiMask myAffected, VectorPiMask opAffected) {
    constexpr Side Op{~My};

    updateSliderAttacks<My>(myAffected);

    OP.setSliderAttacks(opAffected & OP.sliders(), OP.occupied());
}

template <Side::_t My>
void Position::setGamePhase() {
    constexpr Side Op{~My};
    //game gamePhase (middlegame or endgame) depends on opponents pieces count
    MY.setGamePhase(OP.generateGamePhase());
}

template <Side::_t My>
void Position::capture(Square from) {
    constexpr Side Op{~My};
    MY.capture(from);
    setGamePhase<Op>();
}

template <Side::_t My>
void Position::movePawn(Pi pi, Square from, Square to) {
    constexpr Side Op{~My};

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (MY.isPawn(pi));

    MY.movePawn(pi, from, to);
}

template <Side::_t My>
void Position::movePiece(Pi pi, Square from, Square to) {
    constexpr Side Op{~My};

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (!MY.isPawn(pi));

    PieceType ty{MY.typeOf(pi)};
    assert (!ty.is(King));

    MY.move(pi, ty, from, to);
    if (MY.isSlider(pi)) {
        MY.updatePinner(pi);
    }
}

template <Side::_t My>
void Position::playCastling(Pi rook, Square rookFrom, Square kingFrom) {
    constexpr Side Op{~My};

    Square kingTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide) ? C1 : G1;
    Square rookTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide) ? D1 : F1;

    MY.castle(rook, rookFrom, rookTo, kingFrom, kingTo);
    MY.updatePinner(rook);
    OP.setOpKing(~kingTo);

    //TRICK: castling should not affect opponent's sliders, otherwise it is check or pin
    //TRICK: castling rook should attack 'kingFrom' square
    //TRICK: only first rank sliders can be affected
    updateSliderAttacks<My>(MY.attacksTo(rookFrom, kingFrom) & MY.piecesOn(Rank1));

    MY.assertValid(TheKing);
    MY.assertValid(rook);
}

template <Side::_t My>
bool Position::isPinned(Bb allOccupiedWithoutPinned) const {
    constexpr Side Op{~My};

    for (Pi pinner : MY.pinners()) {
        Bb pinLine = ::between(MY.opKingSquare(), MY.squareOf(pinner));
        if ((pinLine & allOccupiedWithoutPinned).none()) {
            return true;
        }
    }

    return false;
}

template <Side::_t My>
void Position::setLegalEnPassant(Pi pi) {
    constexpr Side Op{~My};

    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    Square to{MY.squareOf(pi)};
    Square from(File{to}, Rank2);
    Square ep(File{to}, Rank3);

    assert (to.is(Rank4));

    Bb killers = ~OP.occupiedByPawns() & ::pieceTypeAttack(Pawn, ep);
    if (killers.none()) {
        return;
    }

    if (isPinned<My>(OCCUPIED)) {
        assert ((MY.attacksToKing() % pi).any());
        return; //discovered check found
    }
    assert ((MY.attacksToKing() % pi).none());

    for (Square killer : killers) {
        assert (killer.is(Rank4));

        if (!isPinned<My>(OCCUPIED - killer + ep - to)) {
            MY.setEnPassantVictim(pi);
            OP.setEnPassantKiller(OP.pieceOn(~killer));
        }
    }
}

template <Side::_t My>
void Position::playPawnMove(Pi pi, Square from, Square to) {
    constexpr Side Op{~My};

    if (from.is(Rank7)) {
        //decoding promotion piece type and destination square
        PromoType ty{to};
        Square promotedTo = Square(File(to), Rank8);

        MY.promote(pi, ty, from, promotedTo);
        if (isSlider(ty)) {
            MY.updatePinner(pi);
        }
        setGamePhase<Op>();

        if (OP.isOccupied(~promotedTo)) {
            //promotion with capture
            capture<Op>(~promotedTo);
            updateSliderAttacks<My>(MY.attacksTo(from) | pi, OP.attacksTo(~from));
            return;
        }

        //promotion without capture
        updateSliderAttacks<My>(MY.attacksTo(from, promotedTo) | pi, OP.attacksTo(~from, ~promotedTo));
        return;
    }

    if (OP.isOccupied(~to)) {
        capture<Op>(~to);

        if (from.is(Rank5) && to.is(Rank5)) {
            //en passant move
            Square ep(File{to}, Rank6);
            movePawn<My>(pi, from, ep);
            updateSliderAttacks<My>(MY.attacksTo(from, to, ep), OP.attacksTo(~from, ~to, ~ep));
            return;
        }

        //simple pawn capture
        movePawn<My>(pi, from, to);
        updateSliderAttacks<My>(MY.attacksTo(from), OP.attacksTo(~from));
        return;
    }

    //simple pawn move
    movePawn<My>(pi, from, to);

    updateSliderAttacks<My>(MY.attacksTo(from, to), OP.attacksTo(~from, ~to));

    if (from.is(Rank2) && to.is(Rank4)) {
        setLegalEnPassant<My>(pi);
    }
}

template <Side::_t My>
void Position::playKingMove(Square from, Square to) {
    constexpr Side Op{~My};

    Pi pi = TheKing;

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (MY.typeOf(pi).is(King));

    MY.moveKing(from, to);
    OP.setOpKing(~to);

    if (OP.isOccupied(~to)) {
        //capture
        capture<Op>(~to);
        updateSliderAttacks<My>(MY.attacksTo(from));
        return;
    }

    //non-capture
    updateSliderAttacks<My>(MY.attacksTo(from, to));
}

template <Side::_t My>
void Position::playMove(Square from, Square to) {
    constexpr Side Op{~My};

    //Assumes that the given move is valid and legal
    //En Passant capture encoded as the pawn captures the pawn
    //Castling move encoded as rook captures the king

    //clear en passant status from the previous move
    if (OP.hasEnPassant()) {
        OP.clearEnPassantVictim();
        MY.clearEnPassantKillers();
    }
    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    Pi pi{MY.pieceOn(from)};

    if (MY.isPawn(pi)) {
        playPawnMove<My>(pi, from, to);
        return;
    }

    if (MY.kingSquare().is(from)) {
        playKingMove<My>(from, to);
        return;
    }

    if (OP.isOccupied(~to)) {
        //simple non-pawn non-king capture
        capture<Op>(~to);
        movePiece<My>(pi, from, to);
        updateSliderAttacks<My>(MY.attacksTo(from) | pi, OP.attacksTo(~from));
        return;
    }

    if (MY.kingSquare().is(to)) {
        //castling move encoded as castling rook captures own king
        playCastling<My>(pi, from, to);
        return;
    }

    //simple non-pawn non-king non-capture move
    movePiece<My>(pi, from, to);
    updateSliderAttacks<My>(MY.attacksTo(from, to), OP.attacksTo(~from, ~to));
}

void Position::playMove(const Position& parent, Square from, Square to, Zobrist z) {
    zobrist = z;

    assert (this != &parent);
    MY = parent.OP;
    OP = parent.MY;

    //current position flipped its sides relative to parent, so we make the move inplace for the Op
    playMove<Op>(from, to);

    //king cannot be in check
    assert (MY.attacksToKing().none());
}

void Position::playMove(Square from, Square to) {
    zobrist = createZobrist(from, to);
    PositionSide::swap(MY, OP);

    //the position just swapped its sides, so we make the move for the Op
    playMove<Op>(from, to);

    //king cannot be in check
    assert (MY.attacksToKing().none());
}

Zobrist Position::generateZobrist() const {
    auto mz = MY.generateZobrist();
    auto oz = OP.generateZobrist();

    if (OP.hasEnPassant()) {
        oz.setEnPassant(OP.enPassantSquare());
    }

    return Zobrist(mz, oz);
}

Zobrist Position::createZobrist(Square from, Square to) const {
    Zobrist mz{zobrist};
    Zobrist oz{0};

    if (OP.hasEnPassant()) {
        oz.clearEnPassant(OP.enPassantSquare());
    }

    Pi pi {MY.pieceOn(from)};
    PieceType ty {MY.typeOf(pi)};

    if (ty.is(Pawn)) {
        if (from.is(Rank7)) {
            Square promotedTo = Square(File(to), Rank8);
            mz.clear(Pawn, from);
            mz.drop(static_cast<PieceType>(PromoType{to}), promotedTo);

            if (OP.isOccupied(~promotedTo)) {
                Pi victim {OP.pieceOn(~promotedTo)};

                if (OP.isCastling(victim)) {
                    oz.clearCastling(~promotedTo);
                }
                oz.clear(OP.typeOf(victim), ~promotedTo);
            }
            return Zobrist{oz, mz};
        }

        if (from.is(Rank2) && to.is(Rank4)) {
            mz.move(ty, from, to);

            File file = File{from};
            Square ep(file, Rank3);

            Bb killers = ~OP.occupiedByPawns() & ::pieceTypeAttack(Pawn, ep);
            if (killers.any() && !isPinned<My>(OCCUPIED - from + ep)) {
                for (Square killer : killers) {
                    assert (killer.is(Rank4));

                    if (!isPinned<My>(OCCUPIED - killer + ep)) {
                        mz.setEnPassant(file);
                        return Zobrist{oz, mz};
                    }
                }
            }
            return Zobrist{oz, mz};
        }

        if (from.is(Rank5) && to.is(Rank5)) {
            Square ep(File{to}, Rank6);
            mz.move(Pawn, from, ep);
            oz.clear(Pawn, ~to);
            return Zobrist{oz, mz};
        }

        //the rest of pawns moves (non-promotion, non en passant, non double push)
    }
    else if (MY.kingSquare().is(to)) {
        //castling move encoded as rook moves over own king's square
        for (Pi rook : MY.castlingRooks()) {
            mz.clearCastling(MY.squareOf(rook));
        }

        Square kingFrom = to;
        Square rookFrom = from;
        Square kingTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide) ? C1 : G1;
        Square rookTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide) ? D1 : F1;

        mz.clear(King, kingFrom);
        mz.clear(Rook, rookFrom);
        mz.drop(King, kingTo);
        mz.drop(Rook, rookTo);
        return Zobrist{oz, mz};
    }
    else if (ty.is(King)) {
        for (Pi rook : MY.castlingRooks()) {
            mz.clearCastling(MY.squareOf(rook));
        }
    }
    else if (MY.isCastling(pi)) {
        //move of the rook with castling rights
        assert (ty.is(Rook));
        mz.clearCastling(from);
    }

    if (OP.isOccupied(~to)) {
        Pi victim {OP.pieceOn(~to)};
        oz.clear(OP.typeOf(victim), ~to);

        if (OP.isCastling(victim)) {
            oz.clearCastling(~to);
        }
    }

    mz.move(ty, from, to);
    return Zobrist{oz, mz};
}

bool Position::setEnPassant(File file) {
    if (MY.hasEnPassant() || OP.hasEnPassant()) { return false; }
    if (OCCUPIED[Square(file, Rank7)]) { return false; }
    if (OCCUPIED[Square(file, Rank6)]) { return false; }

    Square victimSquare(file, Rank4);
    if (!OP.isPieceOn(victimSquare)) { return false; }

    Pi victim{OP.pieceOn(victimSquare)};
    if (!OP.isPawn(victim)) { return false; }

    //check against illegal en passant set field like "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6"
    if (isPinned<Op>(OP.occupied() - victimSquare)) {
        return false;
    }

    setLegalEnPassant<Op>(victim);
    return true;
}

#undef MY
#undef OP
#undef OCCUPIED
