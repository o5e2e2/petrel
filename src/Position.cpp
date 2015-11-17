#include "Position.hpp"
#include <utility>
#include "CastlingRules.hpp"

#define MY side[My]
#define OP side[Op]
#define OCCUPIED occupied[My]

void Position::syncSides() {
    occupied = DualBbOccupied(MY.occupiedSquares(), OP.occupiedSquares());
}

template <Side::_t My>
void Position::updateSliderAttacksKing(VectorPiMask affected) {
    constexpr Side Op{~My};

    syncSides();

    affected &= MY.sliders();
    MY.updateSliderAttacks(affected, OCCUPIED - ~OP.kingSquare());
}

template <Side::_t My>
void Position::updateSliderAttacks(VectorPiMask affected) {
    constexpr Side Op{~My};

    affected &= MY.sliders();
    MY.updateSliderAttacks(affected, OCCUPIED);

    assert (MY.attacksTo(~OP.kingSquare()).none());
}

bool Position::setup() {
    updateSliderAttacksKing<Op>(OP.alivePieces());
    updateSliderAttacks<My>(MY.alivePieces());
    return MY.attacksTo(~OP.kingSquare()).none(); //not in check
}

void Position::setZobrist() {
    zobrist = getZobrist();
}

bool Position::drop(Side My, PieceType ty, Square to) {
    const Side Op{~My};

    if (OCCUPIED[to]) { return false; }
    if ( ty.is(Pawn) && (to.is(Rank1) || to.is(Rank8)) ) { return false; }

    if (!ty.is(King)) {
        Pi pi{ (MY.alivePieces() | Pi{TheKing}).seekVacant() };

        MY.drop(pi, ty, to);
        set(My, pi, ty, to);

        MY.assertValid(pi);
        assert (MY.squareOf(pi).is(to));
        assert (MY.typeOf(pi).is(ty));
    }
    else {
        //TRICK: king should be dropped before any opponents non-king pieces
        MY.drop(TheKing, King, to);
        MY.setLeaperAttack(TheKing, King, to);
        MY.assertValid(TheKing);
    }
    return true;
}

bool Position::setCastling(Side My, File file) {
    return MY.setCastling(file);
}

bool Position::setCastling(Side My, CastlingSide castlingSide) {
    return MY.setCastling(castlingSide);
}

bool Position::setEnPassant(File epFile) {
    if (OCCUPIED[Square(epFile, Rank7)]) { return false; }
    if (OCCUPIED[Square(epFile, Rank6)]) { return false; }

    Square victimSquare(epFile, Rank4);
    if (!OP.isPieceOn(victimSquare)) { return false; }
    Pi victim{OP.pieceOn(victimSquare)};

    if (!OP.isPawn(victim)) { return false; }

    //check against illegal en passant set field like "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6"
    for (Pi pinner : OP.pinnerCandidates() & OP.attacksTo(victimSquare)) {
        Bb pinRay = pinRayFrom<Op>(pinner);
        if (pinRay[~victimSquare]) {
            Bb betweenPieces{(pinRay & OCCUPIED) - ~victimSquare};
            if (betweenPieces.none()) { return false; }
        }
    }

    setLegalEnPassant<Op>(victim);
    return true;
}

template <Side::_t My>
const Bb& Position::pinRayFrom(Pi pi) const {
    constexpr Side Op{~My};
    assert (MY.isSlider(pi));
    assert (MY.pinnerCandidates()[pi]);

    return OP.pinRayFrom(~MY.squareOf(pi));
}

template <Side::_t My>
bool Position::isLegalEnPassant(Pi killer, File epFile) const {
    constexpr Side Op{~My};

    MY.assertValid(killer);
    assert (MY.isPawn(killer));
    Square from{ MY.squareOf(killer) };
    assert (from.is(Rank5));

    Square to(epFile, Rank6);
    assert (MY.allAttacks()[killer][to]);

    if (!MY.kingSquare().is(Rank5)) {
        for (Pi pinner : OP.pinnerCandidates() & OP.attacksTo(~from)) {
            Bb pinRay = pinRayFrom<Op>(pinner);
            if (pinRay[from] && !pinRay[to]) {
                Bb betweenPieces{(pinRay & OCCUPIED) - from};
                if (betweenPieces.none()) { return false; } //the true diagonal pin
            }
        }
    }
    else {
        for (Pi pinner : OP.pinnerCandidates() & OP.piecesOn(Rank4)) {
            Bb pinRay = pinRayFrom<Op>(pinner);
            if (pinRay[from]) {
                Square victim(epFile, Rank5);
                assert (OP.isPawn(OP.pieceOn(~victim)));
                Bb betweenPieces{(pinRay & OCCUPIED) - from - victim};
                if (betweenPieces.none()) { return false; } //the true vertical pin
            }
        }
    }
    return true;
}

Zobrist Position::getZobrist() const {
    return Zobrist(MY.getZobrist(), OP.getZobrist());
}

template <Side::_t My>
void Position::setLegalEnPassant(Pi pi) {
    constexpr Side Op{~My};

    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    File epFile = File{MY.squareOf(pi)};

    auto killers = OP.pawns() & OP.attacksTo(Square(epFile, Rank6));

    if (killers.none()) {
        return;
    }

    if ( (MY.attacksTo(~OP.kingSquare()) % pi).any() ) {
        return; //discovered check
    }

    for (Pi killer : killers) {
        if (isLegalEnPassant<Op>(killer, epFile)) {
            OP.markEnPassant(killer);
        }
    }

    if (OP.hasEnPassant()) {
        MY.setEnPassant(pi, epFile);
        //syncZobrist();
    }
}

void Position::set(Side My, Pi pi, PieceType ty, Square to) {
    const Side Op{~My};

    assert (!ty.is(King));

    if (MY.isSlider(pi)) {
        MY.updatePinRays(~OP.kingSquare(), pi);

        //slider attacks updated separately
    }
    else {
        MY.setLeaperAttack(pi, ty, to);
    }
}

template <Side::_t My>
void Position::movePawn(Pi pi, Square from, Square to) {
    constexpr Side Op{~My};

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (MY.isPawn(pi));

    MY.movePawn(pi, from, to);
    MY.setLeaperAttack(pi, Pawn, to);

    MY.assertValid(pi);
}

template <Side::_t My>
void Position::move(Pi pi, Square from, Square to) {
    constexpr Side Op{~My};

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (!MY.isPawn(pi));

    PieceType ty{MY.typeOf(pi)};
    assert (!ty.is(King));

    MY.move(pi, ty, from, to);
    set(My, pi, ty, to);

    MY.assertValid(pi);
}

template <Side::_t My>
void Position::makeKingMove(Square from, Square to) {
    constexpr Side Op{~My};

    Pi pi = TheKing;

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (MY.typeOf(pi).is(King));

    MY.moveKing(from, to);
    MY.setLeaperAttack(TheKing, King, to);
    OP.updatePinRays(~to);

    MY.assertValid(pi);

    if (OP.isOccupied(~to)) {
        //capture
        OP.capture(~to);
        updateSliderAttacksKing<My>(MY.attacksTo(from));
    }
    else {
        //non-capture
        updateSliderAttacksKing<My>(MY.attacksTo(from, to));
    }

}

template <Side::_t My>
void Position::makeCastling(Pi rook, Square rookFrom, Square kingFrom) {
    constexpr Side Op{~My};

    Square kingTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? C1 : G1;
    Square rookTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? D1 : F1;

    MY.castle(rook, rookFrom, rookTo, kingFrom, kingTo);
    MY.setLeaperAttack(TheKing, King, kingTo);

    MY.updatePinRays(~OP.kingSquare(), rook);
    OP.updatePinRays(~kingTo);

    //TRICK: castling should not affect opponent's sliders, otherwise it is check or pin
    //TRICK: castling rook should attack 'kingFrom' square
    //TRICK: only first rank sliders can be affected
    updateSliderAttacksKing<My>(MY.attacksTo(rookFrom, kingFrom) & MY.piecesOn(Rank1));

    MY.assertValid(TheKing);
    MY.assertValid(rook);
}

template <Side::_t My>
void Position::makePawnMove(Pi pi, Square from, Square to) {
    constexpr Side Op{~My};

    if (from.is(Rank7)) {
        //decoding promotion piece type and destination square
        PromoType ty = Move::decodePromoType(to);
        Square promoted_to = Square(File(to), Rank8);

        MY.promote(pi, ty, from, promoted_to);
        set(My, pi, static_cast<PieceType>(ty), promoted_to);

        if (OP.isOccupied(~promoted_to)) {
            OP.capture(~promoted_to);

            updateSliderAttacksKing<My>(MY.attacksTo(from) | pi);
            updateSliderAttacks<Op>(OP.attacksTo(~from));
        }
        else {
            updateSliderAttacksKing<My>(MY.attacksTo(from, promoted_to) | pi);
            updateSliderAttacks<Op>(OP.attacksTo(~from, ~promoted_to));
        }

    }
    else if (OP.isOccupied(~to)) {
        OP.capture(~to);

        if (from.is(Rank5) && to.is(Rank5)) {
            //en passant move
            Square _to(File{to}, Rank6);
            movePawn<My>(pi, from, _to);
            updateSliderAttacksKing<My>(MY.attacksTo(from, to, _to));
            updateSliderAttacks<Op>(OP.attacksTo(~from, ~to, ~_to));
        }
        else {
            //simple pawn capture
            movePawn<My>(pi, from, to);
            updateSliderAttacksKing<My>(MY.attacksTo(from));
            updateSliderAttacks<Op>(OP.attacksTo(~from));
        }
    }
    else {
        //simple pawn move
        movePawn<My>(pi, from, to);

        updateSliderAttacksKing<My>(MY.attacksTo(from, to));
        updateSliderAttacks<Op>(OP.attacksTo(~from, ~to));

        if (from.is(Rank2) && to.is(Rank4)) {
            setLegalEnPassant<My>(pi);
        }

    }

    MY.assertValid(pi);
}

void Position::makeMove(Zobrist z, const Position& parent, Square from, Square to) {
    zobrist = z;

    if (this == &parent) {
        PositionSide::swap(MY, OP);
    }
    else {
        MY = parent.OP;
        OP = parent.MY;
    }

    //PRE: position just flipped sides, so we make the parent move for the opposite side to move
    makeMove<Op>(from, to);
}

template <Side::_t My>
void Position::makeMove(Square from, Square to) {
    constexpr Side Op{~My};

    //Assumes that the given move is valid and legal
    //En Passant capture encoded as the pawn captures the pawn
    //Castling move encoded as rook captures the king

    //clear en passant status from the previous move
    if (OP.hasEnPassant()) {
        OP.clearEnPassant();
        MY.unmarkEnPassants();
    }
    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    Pi pi{MY.pieceOn(from)};

    if (MY.isPawn(pi)) {
        makePawnMove<My>(pi, from, to);
    }
    else if (MY.kingSquare().is(from)) {
        makeKingMove<My>(from, to);
    }
    else if (OP.isOccupied(~to)) {
        //simple non-pawn capture
        OP.capture(~to);
        move<My>(pi, from, to);
        updateSliderAttacksKing<My>(MY.attacksTo(from) | pi);
        updateSliderAttacks<Op>(OP.attacksTo(~from));
    }
    else {
        if (MY.kingSquare().is(to)) {
            makeCastling<My>(pi, from, to);
        }
        else {
            //simple non-pawn move
            move<My>(pi, from, to);
            updateSliderAttacksKing<My>(MY.attacksTo(from, to));
            updateSliderAttacks<Op>(OP.attacksTo(~from, ~to));
        }
    }
}

bool Position::isLegalEnPassantBefore(Pi killer, File epFile) const {
    OP.assertValid(killer);
    assert (OP.isPawn(killer));
    Square from{ OP.squareOf(killer) };
    assert (from.is(Rank5));

    Square to(epFile, Rank6);
    assert (OP.allAttacks()[killer][to]);

    for (Pi pinner : MY.pinnerCandidates()) {
        Bb pinRay = pinRayFrom<My>(pinner);
        if (pinRay[from] && !pinRay[to]) {
            Bb betweenPieces{(pinRay & ~OCCUPIED) - from};
            if (betweenPieces.none()) { return false; }
        }
    }
    return true;
}

Zobrist Position::makeZobrist(Square from, Square to) const {
    Zobrist mz = {};
    Zobrist oz = ~zobrist;

    if (OP.hasEnPassant()) {
        oz.clearEnPassant(OP.enPassantSquare());
    }

    Pi pi {MY.pieceOn(from)};
    PieceType ty {MY.typeOf(pi)};

    if (ty == Pawn) {
        if (from.is(Rank7)) {
            Square promoted_to = Square(File(to), Rank8);
            mz.clear(Pawn, from);
            mz.drop(Move::decodePromoType(to), promoted_to);

            if (OP.isOccupied(~promoted_to)) {
                Pi victim {OP.pieceOn(~promoted_to)};

                if (OP.isCastling(victim)) {
                    oz.clearCastling(~promoted_to);
                }
                oz.clear(OP.typeOf(victim), ~promoted_to);
            }
            return Zobrist{oz, mz};
        }
        else if (from.is(Rank2) && to.is(Rank4)) {
            mz.move(ty, from, to);

            File epFile = File{from};
            auto killers = OP.pawns() & OP.attacksTo(Square(epFile, Rank6));
            if (killers.none()) {
                return Zobrist{oz, mz};
            }

            for (Pi pinner : MY.pinnerCandidates()) {
                Bb pinRay = ~pinRayFrom<My>(pinner);
                if (pinRay[from] && !pinRay[to]) {
                    Bb betweenPieces{(pinRay & OCCUPIED) - from};
                    if (betweenPieces.none()) {
                        return Zobrist{oz, mz};
                    }
                }
            }

            for (Pi killer : killers) {
                if (isLegalEnPassantBefore(killer, epFile)) {
                    mz.setEnPassant(epFile);
                    break;
                }
            }
            return Zobrist{oz, mz};
        }
        else if (from.is(Rank5) && to.is(Rank5)) {
            Square _to(File{to}, Rank6);
            mz.move(Pawn, from, _to);
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
        Square kingTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? C1 : G1;
        Square rookTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? D1 : F1;

        mz.move(King, kingFrom, kingTo);
        mz.move(Rook, rookFrom, rookTo);
        return Zobrist{oz, mz};
    }
    else if (ty == King) {
        for (Pi rook : MY.castlingRooks()) {
            mz.clearCastling(MY.squareOf(rook));
        }
    }
    else if (MY.isCastling(pi)) {
        //move of the rook with castling rights
        assert (ty == Rook);
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

#undef MY
#undef OP
#undef OCCUPIED
