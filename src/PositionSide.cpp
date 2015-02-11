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

        assert (!types.is<Pawn>(pi) || pawnsBb[sq]);
        assert (!types.is<Pawn>(pi) || (!sq.is<Rank1>() && !sq.is<Rank8>()));
        assert (!types.isEnPassant(pi) || sq.is<Rank4>() || sq.is<Rank5>());
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
    if (ty == Pawn) { pawnsBb += to; }
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

    if (ty == Rook && isCastling(pi)) {
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

    if (ty == Rook && isCastling(pi)) {
        clearCastling(pi, from);
    }
    clear(ty, from);

    squares.move(pi, to);
    set(ty, to);

    assertValid(pi);
}

void PositionSide::promote(Pi pi, PromoType ty, Square from, Square to) {
    assertValid(pi);
    assert (types.is<Pawn>(pi));
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

    move(TheKing, King, from, to);
    setLeaperAttack(TheKing, King, to);
    clearCastlings();

    assertValid(TheKing);
}

void PositionSide::castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo) {
    assertValid(rook);
    assertValid(TheKing);
    assert (kingFrom == kingSquare() && kingFrom.is<Rank1>());
    assert (rookFrom == squareOf(rook) && rookFrom.is<Rank1>());

    squares.castle(rook, rookTo, TheKing, kingTo);

    clear(Rook, rookFrom);
    clear(King, kingFrom);
    set(King, kingTo);
    set(Rook, rookTo);

    setLeaperAttack(TheKing, King, kingTo);
    clearCastlings();

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

void PositionSide::clearCastling(Pi pi, Square from) {
    assertValid(pi);
    assert (from == squareOf(pi));
    assert (from.is<Rank1>());

    types.clearCastling(pi);
    zobrist.clearCastling(from);
}

void PositionSide::clearCastlings() {
    for (Pi pi : castlingRooks()) {
        clearCastling(pi, squareOf(pi));
    }
}

void PositionSide::setCastling(Pi rook) {
    types.setCastling(rook);

    Square from = squareOf(rook);
    assert (from.is<Rank1>());
    zobrist.setCastling(from);
    ::castlingRules.set(kingSquare(), from);
}

bool PositionSide::setCastling(CastlingSide side) {
    if (!kingSquare().is<Rank1>()) { return false; }

    auto rooks = of<Rook>() & of<Rank1>();

    Pi theRook;
    if (side == KingSide) {
        auto rightRooks = rooks & squares.rightBackward(kingSquare());
        if (rightRooks.none()) { return false; }
        theRook = rightRooks.last();
    }
    else {
        assert (side == QueenSide);
        auto leftRooks = rooks & squares.leftForward(kingSquare());
        if (leftRooks.none()) { return false; }
        theRook = leftRooks.first();
    }

    setCastling(theRook);
    return true;
}

bool PositionSide::setCastling(File file) {
    if (!kingSquare().is<Rank1>()) { return false; }

    Square rookFrom(file, Rank1);

    if (isOn(rookFrom)) {
        Pi pi{ pieceOn(rookFrom) };
        if (is<Rook>(pi)) {
            setCastling(pi);
            return true;
        }
    }

    return false;
}

void PositionSide::markEnPassant(Pi pi) {
    assert (is<Pawn>(pi));
    types.setEnPassant(pi);
}

void PositionSide::setEnPassant(Pi pi, File fileFrom) {
    markEnPassant(pi);
    assert (squareOf(pi).is<Rank4>());
    assert (File{squareOf(pi)} == fileFrom);

    zobrist.setEnPassant(fileFrom);
}

void PositionSide::clearEnPassant() {
    assert (hasEnPassant());
    assert (types.enPassantPawns().isSingleton());
    assert ((types.enPassantPawns() & squares.of<Rank4>()) == types.enPassantPawns());

    zobrist.clearEnPassant(enPassantFile());
    types.clearEnPassants();
}

void PositionSide::clearEnPassants() {
    assert (hasEnPassant());
    assert ((types.enPassantPawns() & squares.of<Rank5>()) == types.enPassantPawns());
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
