#include "Position.hpp"
#include "BetweenSquares.hpp"
#include "CastlingRules.hpp"
#include "PieceTypeAttack.hpp"

#define MY (*this)[My]
#define OP (*this)[Op]

template <Side::_t My>
void Position::updateSliderAttacks(PiMask affected) {
    constexpr Side Op{~My};

    PositionSide::syncOccupied(MY, OP);
    MY.setSliderAttacks(affected);
}

template <Side::_t My>
void Position::updateSliderAttacks(PiMask myAffected, PiMask opAffected) {
    constexpr Side Op{~My};

    PositionSide::syncOccupied(MY, OP);
    MY.setSliderAttacks(myAffected);
    OP.setSliderAttacks(opAffected);
}

template <Side::_t My>
void Position::capture(Square from) {
    constexpr Side Op{~My};
    MY.capture(from);
    OP.setGamePhase(MY);
}

template <Side::_t My>
void Position::playCastling(Pi rook, Square rookFrom, Square kingFrom) {
    constexpr Side Op{~My};

    Square kingTo = CastlingRules::castlingKingTo(kingFrom, rookFrom);
    Square rookTo = CastlingRules::castlingRookTo(kingFrom, rookFrom);

    MY.castle(kingFrom, kingTo, rook, rookFrom, rookTo);
    OP.setOpKing(~kingTo);

    //TRICK: castling should not affect opponent's sliders, otherwise it is check or pin
    //TRICK: castling rook should attack 'kingFrom' square
    //TRICK: only first rank sliders can be affected
    updateSliderAttacks<My>(MY.attackersTo(rookFrom, kingFrom) & MY.piecesOn(Rank1));

    MY.assertValid(TheKing);
    MY.assertValid(rook);
}

template <Side::_t My>
void Position::setLegalEnPassant(Pi pi) {
    constexpr Side Op{~My};

    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    Square to{MY.squareOf(pi)};
    Square from(File(to), Rank2);
    Square ep(File(to), Rank3);

    assert (to.on(Rank4));

    Bb killers = ~OP.pawnsSquares() & ::attacksFrom(Pawn, ep);
    if (killers.none()) {
        return;
    }

    if (MY.isPinned(MY.occupied())) {
        assert ((MY.checkers() % pi).any());
        return; //discovered check found
    }
    assert ((MY.checkers() % pi).none());

    for (Square killer : killers) {
        assert (killer.on(Rank4));

        if (!MY.isPinned(MY.occupied() - killer + ep - to)) {
            MY.setEnPassantVictim(pi);
            OP.setEnPassantKiller(OP.pieceOn(~killer));
        }
    }
}

template <Side::_t My>
void Position::playPawnMove(Pi pi, Square from, Square to) {
    constexpr Side Op{~My};

    if (from.on(Rank7)) {
        //decoding promotion piece type and destination square
        PromoType ty(to);
        Square promotedTo{File(to), Rank8};

        MY.promote(pi, ty, from, promotedTo);
        OP.setGamePhase(MY);

        if (OP.isOccupied(~promotedTo)) {
            //promotion with capture
            capture<Op>(~promotedTo);
            updateSliderAttacks<My>(MY.attackersTo(from) | pi, OP.attackersTo(~from));
            return;
        }

        //promotion without capture
        updateSliderAttacks<My>(MY.attackersTo(from, promotedTo) | pi, OP.attackersTo(~from, ~promotedTo));
        return;
    }

    if (OP.isOccupied(~to)) {
        capture<Op>(~to);

        //en Passant capture encoded as the pawn captures the pawn
        if (from.on(Rank5) && to.on(Rank5)) {
            Square ep{File(to), Rank6};
            MY.movePawn(pi, from, ep);
            updateSliderAttacks<My>(MY.attackersTo(from, to, ep), OP.attackersTo(~from, ~to, ~ep));
            return;
        }

        //simple pawn capture
        MY.movePawn(pi, from, to);
        updateSliderAttacks<My>(MY.attackersTo(from), OP.attackersTo(~from));
        return;
    }

    //simple pawn move
    MY.movePawn(pi, from, to);
    updateSliderAttacks<My>(MY.attackersTo(from, to), OP.attackersTo(~from, ~to));

    if (from.on(Rank2) && to.on(Rank4)) {
        setLegalEnPassant<My>(pi);
    }
}

template <Side::_t My>
void Position::playKingMove(Square from, Square to) {
    constexpr Side Op{~My};

    MY.moveKing(from, to);
    OP.setOpKing(~to);

    if (OP.isOccupied(~to)) {
        //capture
        capture<Op>(~to);
        updateSliderAttacks<My>(MY.attackersTo(from));
        return;
    }

    //non-capture
    updateSliderAttacks<My>(MY.attackersTo(from, to));
}

template <Side::_t My>
void Position::playMove(Square from, Square to) {
    constexpr Side Op{~My};

    //Assumes that the given move is valid and legal
    OP.clearCheckers();
    assert (MY.checkers().none());

    //clear en passant status from the previous move
    if (OP.hasEnPassant()) {
        OP.clearEnPassantVictim();
        MY.clearEnPassantKillers();
    }
    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    Pi pi = MY.pieceOn(from);

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
        MY.move(pi, from, to);
        updateSliderAttacks<My>(MY.attackersTo(from) | pi, OP.attackersTo(~from));
        return;
    }

    if (MY.kingSquare().is(to)) {
        //castling move encoded as castling rook captures own king
        playCastling<My>(pi, from, to);
        return;
    }

    //simple non-pawn non-king non-capture move
    MY.move(pi, from, to);
    updateSliderAttacks<My>(MY.attackersTo(from, to), OP.attackersTo(~from, ~to));
}

void Position::playMove(const Position& parent, Square from, Square to) {
    assert (this != &parent);
    MY = parent[Op];
    OP = parent[My];

    //current position flipped its sides relative to parent, so we make the move inplace for the Op
    playMove<Op>(from, to);

    //king cannot be left in check
    assert (MY.checkers().none());
}

void Position::playMove(Square from, Square to) {
    PositionSide::swap(MY, OP);

    //the position just swapped its sides, so we make the move for the Op
    playMove<Op>(from, to);

    //king cannot be left in check
    assert (MY.checkers().none());
}

bool Position::dropValid(Side My, PieceType ty, Square to) {
    return MY.dropValid(ty, to);
}

bool Position::afterDrop() {
    PositionSide::finalSetup(MY, OP);
    updateSliderAttacks<Op>(OP.alivePieces(), MY.alivePieces());
    return MY.checkers().none(); //not in check
}

#undef MY
#undef OP
