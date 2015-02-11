#include "Position.hpp"
#include "io.hpp"

#include <utility>

#define MY side[My]
#define OP side[Op]
#define OCCUPIED occupied[My]

Position::Position (const Position& parent, int) {
    assert (this != &parent);
    MY = parent.OP;
    OP = parent.MY;
}

Position::Position (const Position& parent, Square from, Square to)
    : Position(parent, 0)
{
    //PRE: child position has already flipped sides, so we make the given move for the opposite side to move
    makeMove<Op>(from, to);
}

Position::Position () : side{}, occupied{}, zobrist{} {}

void Position::swapSides() {
    using std::swap;
    PositionSide::swap(MY, OP);
    swap(occupied[My], occupied[Op]);
    zobrist.flip();
}

void Position::syncSides() {
    occupied[My] = MY.occ() + ~OP.occ();
    occupied[Op] = ~occupied[My];
    zobrist = PositionSide::zobrist_combine(MY, OP);
}

template <Side::_t My>
void Position::updateSliderAttacksKing(VectorPiMask affected) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    syncSides();

    affected &= MY.sliders();
    if (affected.any()) {
        MY.updateSliderAttacks(affected, OCCUPIED - ~OP.kingSquare());
    }
}

template <Side::_t My>
void Position::updateSliderAttacks(VectorPiMask affected) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    affected &= MY.sliders();
    if (affected.any()) {
        MY.updateSliderAttacks(affected, OCCUPIED);
    }
    assert (MY.attacksTo(~OP.kingSquare()).none());
}

bool Position::setup() {
    updateSliderAttacksKing<Op>(OP.alive());
    updateSliderAttacks<My>(MY.alive());
    return MY.attacksTo(~OP.kingSquare()).none(); //not in check
}

bool Position::drop(Side My, PieceType ty, Square to) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    if (OCCUPIED[to]) { return false; }
    if ( ty == Pawn && (to.is<Rank1>() || to.is<Rank8>()) ) { return false; }

    if (ty != King) {
        Pi pi{ (MY.alive() | Pi{TheKing}).seekVacant() };

        MY.drop(pi, ty, to);
        set(My, pi, ty, to);

        MY.assertValid(pi);
        assert (to == MY.squareOf(pi));
        assert (ty == MY.typeOf(pi));
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

template <Side::_t My>
void Position::clearEnPassant() {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    if (MY.hasEnPassant()) {
        MY.clearEnPassant();
        OP.clearEnPassants();
    }
    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());
}

template <Side::_t My>
const Bb& Position::pinRayFrom(Pi pi) const {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};
    assert (OP.isSlider(pi));
    assert (OP.pinnerCandidates()[pi]);

    return MY.pinRayFrom(~OP.squareOf(pi));
}

bool Position::setEnPassant(Square ep) {
    if (!ep.is<Rank3>()) { return false; }
    if (OCCUPIED[~ep]) { return false; }
    if (OCCUPIED[~(ep.rankDown())]) { return false; }

    Square victimSquare{ep.rankUp()};

    if (!OP.isOn(victimSquare)) { return false; }
    Pi victim{OP.pieceOn(victimSquare)};

    if (!OP.is<Pawn>(victim)) { return false; }

    //check against illegal en passant set field like "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6"
    for (Pi pi : OP.pinnerCandidates() & OP.attacksTo(victimSquare)) {
        auto pinRay = pinRayFrom<My>(pi);
        if (pinRay[~victimSquare]) {
            Bb betweenPieces{(pinRay & OCCUPIED) - ~victimSquare};
            if (betweenPieces.none()) { return false; }
        }
    }

    setLegalEnPassant<Op>(victim);
    return true;
}

template <Side::_t My>
bool Position::isLegalEnPassant(Pi killer, Square to) const {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    MY.assertValid(killer);
    assert (MY.is<Pawn>(killer));
    assert (MY.allAttacks()[killer][to]);
    assert (to.is<Rank6>());

    Square from{ MY.squareOf(killer) };
    assert (from.is<Rank5>());

    if (!MY.kingSquare().is<Rank5>()) {
        for (Pi pi : OP.pinnerCandidates() & OP.attacksTo(~from)) {
            auto pinRay = pinRayFrom<My>(pi);
            if (pinRay[from] && !pinRay[to]) {
                Bb betweenPieces{(pinRay & OCCUPIED) - from};
                if (betweenPieces.none()) { return false; } //the true diagonal pin
            }
        }
    }
    else {
        for (Pi pi : OP.pinnerCandidates() & OP.of<Rank4>()) {
            auto pinRay = pinRayFrom<My>(pi);
            if (pinRay[from]) {
                Square victim{to.rankDown()};
                Bb betweenPieces{(pinRay & OCCUPIED) - from - victim};
                if (betweenPieces.none()) { return false; } //the true vertical pin
            }
        }
    }
    return true;
}

template <Side::_t My>
void Position::setLegalEnPassant(Pi victim) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    Square ep{~MY.squareOf(victim)};
    assert (ep.is<Rank5>());

    Square to{ep.rankUp()};
    assert (to.is<Rank6>());

    if ( (MY.attacksTo(~OP.kingSquare()) % victim).any() ) {
        return; //discovered check
    }

    for (Pi pi : OP.pawns() & OP.attacksTo(to)) {
        if (isLegalEnPassant<Op>(pi, to)) {
            OP.markEnPassant(pi);
        }
    }

    if (OP.hasEnPassant()) {
        MY.setEnPassant(victim, ~ep);
    }
}

void Position::set(Side My, Pi pi, PieceType ty, Square to) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    assert (ty != King);

    if (MY.isSlider(pi)) {
        MY.updatePinRays(~OP.kingSquare(), pi);

        //slider attacks updated separately
    }
    else {
        MY.setLeaperAttack(pi, ty, to);
    }
}

template <Side::_t My>
void Position::move(Pi pi, Square from, Square to) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    MY.assertValid(pi);
    assert (from == MY.squareOf(pi));

    PieceType ty{MY.typeOf(pi)};
    assert (ty != King);

    MY.move(pi, ty, from, to);
    set(My, pi, ty, to);

    MY.assertValid(pi);
}

template <Side::_t My>
void Position::makeKingMove(Square from, Square to) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    Pi pi = TheKing;

    MY.assertValid(pi);
    assert (from == MY.squareOf(pi));
    assert ( MY.typeOf(pi) == King );

    MY.moveKing(from, to);
    OP.updatePinRays(~to);

    MY.assertValid(pi);

    if (OP[~to]) {
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
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};
    //castling encoded as the given rook captures the own king

    Square kingTo{(rookFrom < kingFrom)? C1:G1};
    Square rookTo{(rookFrom < kingFrom)? D1:F1};

    MY.castle(rook, rookFrom, rookTo, kingFrom, kingTo);

    MY.updatePinRays(~OP.kingSquare(), rook);
    OP.updatePinRays(~kingTo);

    //TRICK: castling should not affect opponent's sliders, otherwise it is check or pin
    //TRICK: castling rook should attack 'kingFrom' square
    //TRICK: only first rank sliders can be affected
    updateSliderAttacksKing<My>(MY.attacksTo(rookFrom, kingFrom) & MY.of<Rank1>());

    MY.assertValid(TheKing);
    MY.assertValid(rook);
}

template <Side::_t My>
void Position::makePawnMove(Pi pi, Square from, Square to) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    if (from.is<Rank7>()) {
        //decoding promotion piece type and destination square
        PromoType ty = Move::decodePromoType(to);
        Square promoted_to = Square(File(to), Rank8);

        MY.promote(pi, ty, from, promoted_to);
        set(My, pi, static_cast<PieceType>(ty), promoted_to);

        if (OP[~promoted_to]) {
            OP.capture(~promoted_to);

            updateSliderAttacksKing<My>(MY.attacksTo(from) | pi);
            updateSliderAttacks<Op>(OP.attacksTo(~from));
        }
        else {
            updateSliderAttacksKing<My>(MY.attacksTo(from, promoted_to) | pi);
            updateSliderAttacks<Op>(OP.attacksTo(~from, ~promoted_to));
        }

    }
    else if (OP[~to]) {
        OP.capture(~to);

        if (from.is<Rank5>() && to.is<Rank5>()) {
            //en passant move
            Square ep{to.rankUp()};
            move<My>(pi, from, ep);
            updateSliderAttacksKing<My>(MY.attacksTo(from, to, ep));
            updateSliderAttacks<Op>(OP.attacksTo(~from, ~to, ~ep));
        }
        else {
            //simple pawn capture
            move<My>(pi, from, to);
            updateSliderAttacksKing<My>(MY.attacksTo(from));
            updateSliderAttacks<Op>(OP.attacksTo(~from));
        }
    }
    else {
        //simple pawn move
        move<My>(pi, from, to);
        updateSliderAttacksKing<My>(MY.attacksTo(from, to));
        updateSliderAttacks<Op>(OP.attacksTo(~from, ~to));

        if (from.is<Rank2>() && to.is<Rank4>()) {
            setLegalEnPassant<My>(pi);
        }
    }

    MY.assertValid(pi);
}

template <Side::_t My>
void Position::makeMove(Square from, Square to) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    //Assumes that the given move is valid and legal
    //En Passant capture encoded as the pawn captures the pawn
    //Castling move encoded as rook captures the king

    //clear en passant status from the previous move
    clearEnPassant<Op>();

    Pi pi{MY.pieceOn(from)};

    if (MY.is<Pawn>(pi)) {
        makePawnMove<My>(pi, from, to);
    }
    else if (from == MY.kingSquare()) {
        makeKingMove<My>(from, to);
    }
    else if (OP[~to]) {
        //simple non-pawn capture
        OP.capture(~to);
        move<My>(pi, from, to);
        updateSliderAttacksKing<My>(MY.attacksTo(from) | pi);
        updateSliderAttacks<Op>(OP.attacksTo(~from));
    }
    else {
        if (to == MY.kingSquare()) {
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

Move readMove(std::istream& in, const Position& pos, Color colorToMove) {
    in >> std::ws;
    if (in.eof()) { return Move::null(); }

    auto before_move = in.tellg();

    Square from{Square::Begin};
    Square to{Square::Begin};
    in >> from >> to;
    if (!in) { io::fail_pos(in, before_move); return Move::null(); }

    if (colorToMove == Black) {
        from.flip();
        to.flip();
    }

    if (!pos.MY.isOn(from) || (from == to)) { io::fail_pos(in, before_move); return Move::null(); }
    Pi pi{pos.MY.pieceOn(from)};

    //convert special moves (castling, promotion, ep) to the internal move format
    if (pos.MY.is<Pawn>(pi)) {
        if (from.is<Rank5>() && pos.OP.hasEnPassant()) {
            Square ep{~pos.OP.enPassantSquare()};
            if (to == ep.rankUp()) {
                return Move::makeSpecial(from, ep);
            }
        }
        else if (from.is<Rank7>()) {
            PromoType promo{PromoType::Begin};
            if (in >> promo) {
                return Move(from, to, promo);
            }
            else { io::fail_pos(in, before_move); return Move::null(); }
        }
    }
    else if (pi == TheKing && from.is<Rank1>() && to.is<Rank1>()) {
        if ( (pos.MY.castlingRooks() & pos.MY.on(to)).any() ) {
            //from Chess960 castling encoding
            return Move::makeCastling(to, from);
        }
        else if (from == E1 && to == G1 && pos.MY[A1]) {
            return Move::makeCastling(A1, E1);
        }
        else if (from == E1 && to == C1 && pos.MY[H1]) {
            return Move::makeCastling(H1, E1);
        }
    }
    return Move(from, to);
}

Move Position::createMove(Side My, Square from, Square to) const {
    const Side Op{~My};

    if ( MY.is<Pawn>(MY.pieceOn(from)) ) {
        if ( from.is<Rank7>() || (OP.hasEnPassant() && to == ~OP.enPassantSquare()) ) {
            return Move::makeSpecial(from, to);
        }
    }
    else if (MY.kingSquare() == to) {
        return Move::makeCastling(from, to);
    }
    return Move(from, to);
}

#undef MY
#undef OP
#undef OCCUPIED
