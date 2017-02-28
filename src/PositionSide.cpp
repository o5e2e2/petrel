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
#endif

PositionSide::PositionSide() : piecesBb{}, pawnsBb{} {
    attacks.clear();
    types.clear();
    squares.clear();
    evaluation.clear();
}

Zobrist PositionSide::generateZobrist() const {
    Zobrist z = {};

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
    swap(my.piecesBb, op.piecesBb);
    swap(my.pawnsBb, op.pawnsBb);
    swap(my.evaluation, op.evaluation);
}

void PositionSide::clear(PieceType ty, Square from) {
    piecesBb -= from;
    evaluation.clear(ty, from);
}

void PositionSide::drop(PieceType ty, Square to) {
    piecesBb += to;
    evaluation.drop(ty, to);
}

void PositionSide::move(PieceType ty, Square from, Square to) {
    piecesBb.move(from, to);
    evaluation.move(ty, from, to);
}

void PositionSide::drop(Pi pi, PieceType ty, Square to) {
    drop(ty, to);
    if (ty.is(Pawn)) { pawnsBb += to; }

    types.drop(pi, ty);
    squares.drop(pi, to);

    assertValid(pi);
}

void PositionSide::capture(Square from) {
    Pi pi{pieceOn(from)};

    assertValid(pi);

    attacks.clear(pi);

    PieceType ty = typeOf(pi);
    assert (!ty.is(King));

    clear(ty, from);
    pawnsBb &= piecesBb; //clear if pawn

    squares.clear(pi);
    types.clear(pi);
}

void PositionSide::move(Pi pi, PieceType ty, Square from, Square to) {
    assert (!ty.is(King));
    assert (!ty.is(Pawn));
    assertValid(pi);
    assert (from != to);

    types.clearCastling(pi);
    move(ty, from, to);
    squares.move(pi, to);

    assertValid(pi);
}

void PositionSide::movePawn(Pi pi, Square from, Square to) {
    assertValid(pi);
    assert (isPawn(pi));
    assert (from != to);

    pawnsBb.move(from, to);
    move(Pawn, from, to);
    squares.move(pi, to);

    assertValid(pi);
}

void PositionSide::moveKing(Square from, Square to) {
    assertValid(TheKing);
    assert (from != to);

    types.clearCastlings();

    move(King, from, to);
    squares.move(TheKing, to);

    assertValid(TheKing);
}

void PositionSide::castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo) {
    assertValid(rook);
    assertValid(TheKing);
    assert (kingSquare().is(kingFrom) && kingFrom.is(Rank1));
    assert (squareOf(rook).is(rookFrom) && rookFrom.is(Rank1));

    types.clearCastlings();

    clear(Rook, rookFrom);
    clear(King, kingFrom);
    drop(King, kingTo);
    drop(Rook, rookTo);

    squares.castle(rook, rookTo, TheKing, kingTo);

    assertValid(TheKing);
    assertValid(rook);
}

void PositionSide::promote(Pi pi, PromoType ty, Square from, Square to) {
    assertValid(pi);
    assert (types.isPawn(pi));
    assert (from.is(Rank7) && to.is(Rank8));

    piecesBb.move(from, to);
    pawnsBb -= from;

    squares.move(pi, to);
    types.promote(pi, ty);

    evaluation.promote(from, to, ty);

    assertValid(pi);
}

void PositionSide::setLeaperAttack(Pi pi, PieceType ty, Square to) {
    assertValid(pi);
    assert (!isSlider(pi));
    attacks.set(pi, ::pieceTypeAttack(ty, to));
}

void PositionSide::updateSliderAttacks(VectorPiMask affectedSliders, Bb occupied) {
    if (affectedSliders.any()) {
        ReverseBb blockers{ occupied };
        for (Pi pi : affectedSliders) {
            Bb attack = blockers.attack(static_cast<SliderType>(typeOf(pi)), squareOf(pi));
            attacks.set(pi, attack);
        }
    }
}

bool PositionSide::setCastling(Pi pi) {
    if (isCastling(pi)) { return false; }

    assert (typeOf(pi).is(Rook));
    assert (squareOf(pi).is(Rank1));

    types.setCastling(pi);

    return true;
}

bool PositionSide::setCastling(CastlingSide castlingSide) {
    if (!kingSquare().is(Rank1)) { return false; }

    Square outerSquare{ kingSquare() };

    for (Pi rook : piecesOfType(Rook) & piecesOn(Rank1)) {
        if (CastlingRules::castlingSide(outerSquare, squareOf(rook)).is(castlingSide)) {
            outerSquare = squareOf(rook);
        }
    }

    if (outerSquare == kingSquare()) { return false; } //no rook found

    return setCastling(pieceOn(outerSquare));
}

bool PositionSide::setCastling(File file) {
    if (!kingSquare().is(Rank1)) { return false; }

    Square rookFrom(file, Rank1);

    if (isPieceOn(rookFrom)) {
        Pi rook { pieceOn(rookFrom) };
        if (is(rook, Rook)) {
            return setCastling(rook);
        }
    }

    return false;
}

void PositionSide::markEnPassant(Pi pi) {
    assert (isPawn(pi));
    assert (squareOf(pi).is(Rank5));
    types.setEnPassant(pi);
}

void PositionSide::unmarkEnPassants() {
    assert (hasEnPassant());
    assert (types.enPassantPawns() <= squares.piecesOn(Rank5));
    types.clearEnPassants();
}

void PositionSide::setEnPassant(Pi pi) {
    assert (isPawn(pi));
    assert (!hasEnPassant());
    assert (squareOf(pi).is(Rank4));
    types.setEnPassant(pi);
}

void PositionSide::clearEnPassant() {
    assert (hasEnPassant());
    assert (types.enPassantPawns().isSingleton());
    assert (types.enPassantPawns() <= squares.piecesOn(Rank4));

    types.clearEnPassants();
}

const Bb& PositionSide::pinRayFrom(Square from) const {
    return ::between(kingSquare(), from);
}

bool PositionSide::canBeAttacked(Square from, Square to) const {
    Pi pi = pieceOn(from);
    bool result = ::pieceTypeAttack(typeOf(pi), from)[to];
    assert (!result || isSlider(pi));
    return result;
}
