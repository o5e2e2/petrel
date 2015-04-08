#include "PositionSide.hpp"
#include "BetweenSquares.hpp"
#include "PieceTypeAttack.hpp"
#include "CastlingRules.hpp"
#include "ReverseBb.hpp"

#ifndef NDEBUG
    void PositionSide::assertValid(Pi pi) const {
        types.assertValid(pi);
        squares.assertValid(pi);

        Square sq = squares.squareOf(pi);
        assert (piecesBb[sq]);

        assert (!types.isPawn(pi) || pawnsBb[sq]);
        assert (!types.isPawn(pi) || (!sq.is(Rank1) && !sq.is(Rank8)));
        assert (!types.isEnPassant(pi) || sq.is(Rank4) || sq.is(Rank5));
    }
#endif

PositionSide::PositionSide() : piecesBb{}, pawnsBb{} {
    attacks.clear();
    types.clear();
    squares.clear();
    zobrist.clear();
    evaluation.clear();
}

void PositionSide::swap(PositionSide& my, PositionSide& op) {
    using std::swap;
    swap(my.attacks, op.attacks);
    swap(my.types, op.types);
    swap(my.squares, op.squares);
    swap(my.piecesBb, op.piecesBb);
    swap(my.pawnsBb, op.pawnsBb);
    swap(my.zobrist, op.zobrist);
    swap(my.evaluation, op.evaluation);
}

void PositionSide::clear(PieceType ty, Square from) {
    piecesBb -= from;
    pawnsBb &= piecesBb;
    zobrist.clear(ty, from);
    evaluation.clear(ty, from);
}

void PositionSide::set(PieceType ty, Square to) {
    piecesBb += to;
    if (ty.is(Pawn)) { pawnsBb += to; }
    zobrist.drop(ty, to);
    evaluation.drop(ty, to);
}

void PositionSide::drop(Pi pi, PieceType ty, Square to) {
    types.drop(pi, ty);
    squares.drop(pi, to);
    set(ty, to);

    assertValid(pi);
}

void PositionSide::capture(Square from) {
    Pi pi{pieceOn(from)};

    assertValid(pi);

    PieceType ty = typeOf(pi);
    assert (!ty.is(King));

    if (isCastling(pi)) {
        zobrist.clearCastling(from);
    }
    clear(ty, from);

    squares.clear(pi);
    types.clear(pi);
    attacks.clear(pi);
}

void PositionSide::move(Pi pi, PieceType ty, Square from, Square to) {
    assertValid(pi);
    assert (from != to);

    if (isCastling(pi)) {
        zobrist.clearCastling(from);
        types.clearCastling(pi);
    }
    clear(ty, from);

    squares.move(pi, to);
    set(ty, to);

    assertValid(pi);
}

void PositionSide::promote(Pi pi, PromoType ty, Square from, Square to) {
    assertValid(pi);
    assert (types.isPawn(pi));
    assert (from != to);

    types.promote(pi, ty);

    squares.move(pi, to);
    piecesBb.move(from, to);
    pawnsBb -= from;

    zobrist.clear(Pawn, from);
    evaluation.clear(Pawn, from);
    zobrist.drop(static_cast<PieceType::_t>(ty), to);
    evaluation.drop(static_cast<PieceType::_t>(ty), to);

    assertValid(pi);
}

void PositionSide::moveKing(Square from, Square to) {
    assertValid(TheKing);

    clearCastlings();

    move(TheKing, King, from, to);
    setLeaperAttack(TheKing, King, to);

    assertValid(TheKing);
}

void PositionSide::castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo) {
    assertValid(rook);
    assertValid(TheKing);
    assert (kingSquare().is(kingFrom) && kingFrom.is(Rank1));
    assert (squareOf(rook).is(rookFrom) && rookFrom.is(Rank1));

    clearCastlings();

    squares.castle(rook, rookTo, TheKing, kingTo);
    clear(Rook, rookFrom);
    clear(King, kingFrom);
    set(King, kingTo);
    set(Rook, rookTo);

    setLeaperAttack(TheKing, King, kingTo);

    assertValid(TheKing);
    assertValid(rook);
}

void PositionSide::setLeaperAttack(Pi pi, PieceType ty, Square to) {
    assertValid(pi);
    assert (!isSlider(pi));
    attacks.set(pi, ::pieceTypeAttack(ty, to));
}

void PositionSide::updateSliderAttacks(VectorPiMask affectedSliders, Bb _occupied) {
    ReverseBb blockers{ _occupied };
    for (Pi pi : affectedSliders) {
        attacks.setSliderAttack(pi, static_cast<SliderType>(typeOf(pi)), squareOf(pi), blockers);
    }
}

void PositionSide::clearCastlings() {
    for (Pi pi : castlingRooks()) {
        zobrist.clearCastling(squareOf(pi));
    }
    types.clearCastlings();
}

bool PositionSide::setCastling(Pi pi) {
    if (isCastling(pi)) { return false; }

    Square from = squareOf(pi);

    assert (typeOf(pi).is(Rook));
    assert (from.is(Rank1));

    zobrist.setCastling(from);
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
        Pi pi{ pieceOn(rookFrom) };
        if (isTypeOf(pi, Rook)) {
            return setCastling(pi);
        }
    }

    return false;
}

void PositionSide::markEnPassant(Pi pi) {
    assert (isPawn(pi));
    types.setEnPassant(pi);
}

void PositionSide::setEnPassant(Pi pi, File fileFrom) {
    assert (!hasEnPassant());
    assert (squareOf(pi).is(Rank4));
    assert (File{squareOf(pi)} == fileFrom);

    markEnPassant(pi);
    zobrist.setEnPassant(fileFrom);
}

void PositionSide::clearEnPassant() {
    assert (hasEnPassant());
    assert (types.enPassantPawns().isSingleton());
    assert (types.enPassantPawns() <= squares.piecesOn(Rank4));

    zobrist.clearEnPassant(enPassantFile());
    types.clearEnPassants();
}

void PositionSide::clearEnPassants() {
    assert (hasEnPassant());
    assert (types.enPassantPawns() <= squares.piecesOn(Rank5));
    types.clearEnPassants();
}

const Bb& PositionSide::pinRayFrom(Square from) const {
    return ::between(kingSquare(), from);
}

void PositionSide::updatePinRays(Square opKingSquare, Pi pi) {
    assert (isSlider(pi));

    if ( ::pieceTypeAttack(typeOf(pi), opKingSquare)[squareOf(pi)] ) {
        types.setPinRay(pi);
    }
    else {
        types.clearPinRay(pi);
    }
}

void PositionSide::updatePinRays(Square opKingSquare) {
    for (Pi pi : sliders()) {
        updatePinRays(opKingSquare, pi);
    }
}
