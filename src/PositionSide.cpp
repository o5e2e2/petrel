#include "PositionSide.hpp"
#include "BetweenSquares.hpp"
#include "CastlingRules.hpp"
#include "PieceTypeAttack.hpp"
#include "ReverseBb.hpp"

#ifndef NDEBUG
    void PositionSide::assertValid(Pi pi) const {
        types.assertValid(pi);
        squares.assertValid(pi);

        Square sq = squares.squareOf(pi);
        assert (piecesBb[sq]);

        assert (!types.isPawn(pi) || pawnsBb[sq]);
        assert (!types.isPawn(pi) || (!sq.is(Rank1) && !sq.is(Rank8)));
        assert (!types.isPawn(pi) || !types.isEnPassant(pi) || sq.is(Rank4) || sq.is(Rank5));
    }

    void PositionSide::assertValid(Pi pi, PieceType ty, Square sq) const {
        assertValid(pi);
        assert (squares.squareOf(pi) == sq);
        assert (types.typeOf(pi) == ty);
    }
#endif

Zobrist PositionSide::generateZobrist() const {
    Zobrist z{0};

    for (Pi pi : alivePieces()) {
        z.drop(typeOf(pi), squareOf(pi));
    }

    for (Pi rook : castlingRooks()) {
        z.setCastling(squareOf(rook));
    }

    return z;
}

void PositionSide::swap(PositionSide& my, PositionSide& op) {
    using std::swap;
    swap(my.attacks, op.attacks);
    swap(my.types, op.types);
    swap(my.squares, op.squares);
    swap(my.occupiedBb, op.occupiedBb);
    swap(my.piecesBb, op.piecesBb);
    swap(my.pawnsBb, op.pawnsBb);
    swap(my.opKing, op.opKing);
    swap(my.evaluation, op.evaluation);
}

void PositionSide::clear(PieceType ty, Square from) {
    piecesBb -= from;
    evaluation.clear(ty, from);
}

void PositionSide::set(PieceType ty, Square to) {
    piecesBb += to;
    evaluation.drop(ty, to);
}

void PositionSide::move(PieceType ty, Square from, Square to) {
    piecesBb.move(from, to);
    evaluation.move(ty, from, to);
}

void PositionSide::capture(Square from) {
    Pi pi{pieceOn(from)};
    assertValid(pi);

    PieceType ty = typeOf(pi);
    assert (!ty.is(King));

    clear(ty, from);
    pawnsBb &= piecesBb; //clear if pawn

    attacks.clear(pi);
    squares.clear(pi);
    types.clear(pi);
}

void PositionSide::move(Pi pi, Square from, Square to) {
    PieceType ty = typeOf(pi);
    assertValid(pi, ty, from);

    assert (!ty.is(King));
    assert (!ty.is(Pawn));
    assert (from != to);

    move(ty, from, to);
    squares.move(pi, to);

    if (ty.is(Knight)) {
        setLeaperAttack(pi, Knight, to);
    }
    else {
        updatePinner(pi);
        types.clearCastling(pi);
    }

    assertValid(pi, ty, to);
}

void PositionSide::movePawn(Pi pi, Square from, Square to) {
    assertValid(pi, Pawn, from);
    assert (from != to);

    move(Pawn, from, to);
    pawnsBb.move(from, to);

    squares.move(pi, to);
    setLeaperAttack(pi, Pawn, to);

    assertValid(pi, Pawn, to);
}

void PositionSide::moveKing(Square from, Square to) {
    assertValid(TheKing, King, from);
    assert (from != to);

    move(King, from, to);
    squares.move(TheKing, to);
    setLeaperAttack(TheKing, King, to);
    types.clearCastlings();

    assertValid(TheKing, King, to);
}

void PositionSide::castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo) {
    assertValid(TheKing, King, kingFrom);
    assertValid(rook, Rook, rookFrom);
    assert (kingFrom.is(Rank1) && rookFrom.is(Rank1));

    clear(Rook, rookFrom);
    clear(King, kingFrom);
    set(King, kingTo);
    set(Rook, rookTo);

    squares.castle(rook, rookTo, TheKing, kingTo);
    setLeaperAttack(TheKing, King, kingTo);
    types.clearCastlings();

    assertValid(TheKing, King, kingTo);
    assertValid(rook, Rook, rookTo);
    assert (kingTo.is(Rank1) && rookTo.is(Rank1));
}

void PositionSide::promote(Pi pi, PromoType ty, Square from, Square to) {
    assertValid(pi, Pawn, from);
    assert (from.is(Rank7));

    piecesBb.move(from, to);
    pawnsBb -= from;

    squares.move(pi, to);
    types.promote(pi, ty);
    evaluation.promote(from, to, ty);

    if (ty.is(Knight)) {
        setLeaperAttack(pi, Knight, to);
    }
    else {
        updatePinner(pi);
    }

    assertValid(pi, static_cast<PieceType>(ty), to);
    assert (to.is(Rank8));
}

void PositionSide::setLeaperAttack(Pi pi, PieceType ty, Square to) {
    assertValid(pi, ty, to);
    assert (isLeaper(ty));
    attacks.set(pi, ::pieceTypeAttack(ty, to));
}

void PositionSide::setSliderAttacks(VectorPiMask affectedSliders, Bb occupied) {
    affectedSliders &= sliders();
    if (affectedSliders.none()) { return; }

    ReverseBb blockers{ occupied };
    for (Pi pi : affectedSliders) {
        Bb attack = blockers.attack(static_cast<SliderType>(typeOf(pi)), squareOf(pi));
        attacks.set(pi, attack);
    }
}

void PositionSide::setEnPassantVictim(Pi pi) {
    assert (isPawn(pi));
    assert (squareOf(pi).is(Rank4));
    assert (!hasEnPassant() || types.isEnPassant(pi));
    types.setEnPassant(pi);
}

void PositionSide::setEnPassantKiller(Pi pi) {
    assert (isPawn(pi));
    assert (squareOf(pi).is(Rank5));
    types.setEnPassant(pi);
}

void PositionSide::clearEnPassantVictim() {
    assert (hasEnPassant());
    assert (types.enPassantPawns().isSingleton());
    assert (types.enPassantPawns() <= squares.piecesOn(Rank4));
    types.clearEnPassants();
}

void PositionSide::clearEnPassantKillers() {
    assert (hasEnPassant());
    assert (types.enPassantPawns() <= squares.piecesOn(Rank5));
    types.clearEnPassants();
}

bool PositionSide::isPinned(Bb allOccupiedWithoutPinned) const {
    for (Pi pinner : pinners()) {
        Bb pinLine = ::between(opKing, squareOf(pinner));
        if ((pinLine & allOccupiedWithoutPinned).none()) {
            return true;
        }
    }

    return false;
}

void PositionSide::updatePinner(Pi pi) {
    assert (isSlider(pi));
    Square sq = squareOf(pi);

    if (::pieceTypeAttack(typeOf(pi), sq)[opKing] && ::between(opKing, sq).any()) {
        types.setPinner(pi);
    }
    else {
        types.clearPinner(pi);
    }
}

bool PositionSide::drop(PieceType ty, Square to) {
    if (piecesBb[to]) { return false; }

    if (ty.is(Pawn)) {
        if (to.is(Rank1) || to.is(Rank8)) { return false; }
        pawnsBb += to;
    }
    set(ty, to);

    Pi pi = ty.is(King) ? Pi{TheKing} : (alivePieces() | Pi{TheKing}).seekVacant();

    types.drop(pi, ty);
    squares.drop(pi, to);

    if (isLeaper(ty)) {
        setLeaperAttack(pi, ty, to);
    }

    assertValid(pi, ty, to);
    return true;
}

bool PositionSide::setCastling(CastlingSide castlingSide) {
    if (!kingSquare().is(Rank1)) { return false; }

    Square outerSquare = kingSquare();
    for (Pi rook : piecesOfType(Rook) & piecesOn(Rank1)) {
        if (CastlingRules::castlingSide(outerSquare, squareOf(rook)).is(castlingSide)) {
            outerSquare = squareOf(rook);
        }
    }
    if (outerSquare == kingSquare()) { return false; } //no rook found

    Pi rook = pieceOn(outerSquare);
    if (isCastling(rook)) { return false; }

    types.setCastling(rook);
    return true;
}

bool PositionSide::setCastling(File file) {
    if (!kingSquare().is(Rank1)) { return false; }

    Square rookFrom(file, Rank1);
    if (!isPieceOn(rookFrom)) { return false; }

    Pi rook = pieceOn(rookFrom);
    if (!is(rook, Rook)) { return false; }
    if (isCastling(rook)) { return false; }

    types.setCastling(rook);
    return true;
}

GamePhase PositionSide::generateGamePhase() const {
    auto queensCount = piecesOfType(Queen).count();
    bool isEndgame = (queensCount == 0) || (queensCount == 1 && types.minors().count() <= 1);
    return isEndgame ? Endgame : Middlegame;
}

void PositionSide::setOpKing(Square sq) {
    opKing = sq;
    for (Pi pi : sliders()) {
        updatePinner(pi);
    }
}

Move PositionSide::createMove(Square from, Square to) const {
    if (kingSquare().is(to)) {
        return Move::castling(from, to);
    }

    if (isPawn(pieceOn(from))) {
        if (from.is(Rank7)) {
            return Move::promotion(from, to);
        }
        if (from.is(Rank5) && to.is(Rank5)) {
            return Move::enPassant(from, to);
        }
    }
    return Move(from, to);
}
