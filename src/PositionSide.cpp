#include "PositionSide.hpp"

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

void PositionSide::clear(Pi pi, PieceType ty, Square from) {
    assertValid(pi);

    attacks.clear(pi);
    types.clear(pi);
    squares.clear(pi);

    clear(ty, from);
}

void PositionSide::capture(Square from) {
    Pi pi{pieceOn(from)};

    assertValid(pi);
    clear(pi, typeOf(pi), from);
}

void PositionSide::drop(PieceType ty, Square to) {
    piecesBb += to;
    if (ty == Pawn) {
        pawnsBb += to;
    }
    zobrist.drop(ty, to);
    evaluation.drop(ty, to);
}

void PositionSide::drop(Pi pi, PieceType ty, Square to) {
    types.drop(pi, ty);
    squares.drop(pi, to);

    drop(ty, to);

    assertValid(pi);
}

void PositionSide::promote(Pi pi, PieceType ty, Square from, Square to) {
    assertValid(pi);
    assert (types.is<Pawn>(pi));
    assert (from != to);

    types.promote(pi, static_cast<PromoType::_t>(ty));

    squares.move(pi, to);
    piecesBb.move(from, to);
    pawnsBb -= from;

    zobrist.clear(Pawn, from);
    evaluation.clear(Pawn, from);
    zobrist.drop(ty, to);
    evaluation.drop(ty, to);

    assertValid(pi);
}

void PositionSide::move(Pi pi, PieceType ty, Square from, Square to) {
    assertValid(pi);
    assert (from != to);

    squares.move(pi, to);
    clear(ty, from);
    drop(ty, to);

    assertValid(pi);
}

void PositionSide::castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo) {
    squares.castle(rook, rookTo, TheKing, kingTo);

    clear(King, kingFrom);
    clear(Rook, rookFrom);
    drop(Rook, rookTo);
    drop(King, kingTo);
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

void PositionSide::clearCastling(Pi pi) {
    assertValid(pi);
    assert (is<Rook>(pi));
    types.clearCastling(pi);
    zobrist.clearCastling(squareOf(pi));
}

void PositionSide::clearCastlings() {
    for (Pi pi : castlingRooks()) {
        clearCastling(pi);
    }
}

void PositionSide::setCastling(Pi rook) {
    assert (is<Rook>(rook));
    types.setCastling(rook);

    Square from = squareOf(rook);
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

void PositionSide::setEnPassant(Pi pi, Square from) {
    markEnPassant(pi);
    zobrist.setEnPassant(from);
}

void PositionSide::clearEnPassant(Pi pi) {
    assert (is<Pawn>(pi));
    assert (squareOf(pi).is<Rank4>());
    types.clearEnPassant(pi);
    zobrist.clearEnPassant(squareOf(pi));
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
