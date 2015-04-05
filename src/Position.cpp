#include "Position.hpp"
#include <utility>
#include "CastlingRules.hpp"

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
    occupied[My] = MY.occupiedSquares() + ~OP.occupiedSquares();
    occupied[Op] = ~occupied[My];
    zobrist = PositionSide::zobrist_combine(MY, OP);
}

template <Side::_t My>
void Position::updateSliderAttacksKing(VectorPiMask affected) {
    const Side::_t Op{~My};

    syncSides();

    affected &= MY.sliders();
    if (affected.any()) {
        MY.updateSliderAttacks(affected, OCCUPIED - ~OP.kingSquare());
    }
}

template <Side::_t My>
void Position::updateSliderAttacks(VectorPiMask affected) {
    const Side::_t Op{~My};

    affected &= MY.sliders();
    if (affected.any()) {
        MY.updateSliderAttacks(affected, OCCUPIED);
    }
    assert (MY.attacksTo(~OP.kingSquare()).none());
}

bool Position::setup() {
    updateSliderAttacksKing<Op>(OP.alivePieces());
    updateSliderAttacks<My>(MY.alivePieces());
    return MY.attacksTo(~OP.kingSquare()).none(); //not in check
}

bool Position::drop(Side My, PieceType ty, Square to) {
    const Side::_t Op{~My};

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

template <Side::_t My>
const Bb& Position::pinRayFrom(Pi pi) const {
    const Side::_t Op{~My};
    assert (OP.isSlider(pi));
    assert (OP.pinnerCandidates()[pi]);

    return MY.pinRayFrom(~OP.squareOf(pi));
}

bool Position::setEnPassant(File epFile) {
    if (OCCUPIED[Square(epFile, Rank7)]) { return false; }
    if (OCCUPIED[Square(epFile, Rank6)]) { return false; }

    Square victimSquare(epFile, Rank4);
    if (!OP.isPieceOn(victimSquare)) { return false; }
    Pi victim{OP.pieceOn(victimSquare)};

    if (!OP.isPawn(victim)) { return false; }

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
bool Position::isLegalEnPassant(Pi killer, File epFile) const {
    const Side::_t Op{~My};

    MY.assertValid(killer);
    assert (MY.isPawn(killer));
    Square from{ MY.squareOf(killer) };
    assert (from.is(Rank5));

    Square to(epFile, Rank6);
    assert (MY.allAttacks()[killer][to]);

    if (!MY.kingSquare().is(Rank5)) {
        for (Pi pi : OP.pinnerCandidates() & OP.attacksTo(~from)) {
            auto pinRay = pinRayFrom<My>(pi);
            if (pinRay[from] && !pinRay[to]) {
                Bb betweenPieces{(pinRay & OCCUPIED) - from};
                if (betweenPieces.none()) { return false; } //the true diagonal pin
            }
        }
    }
    else {
        for (Pi pi : OP.pinnerCandidates() & OP.piecesOn(Rank4)) {
            auto pinRay = pinRayFrom<My>(pi);
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

template <Side::_t My>
void Position::setLegalEnPassant(Pi victim) {
    const Side::_t Op{~My};

    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    File epFile = File{MY.squareOf(victim)};

    auto killers = OP.pawns() & OP.attacksTo(Square(epFile, Rank6));

    if (killers.none()) {
        return;
    }

    if ( (MY.attacksTo(~OP.kingSquare()) % victim).any() ) {
        return; //discovered check
    }

    for (Pi pi : killers) {
        if (isLegalEnPassant<Op>(pi, epFile)) {
            OP.markEnPassant(pi);
        }
    }

    if (OP.hasEnPassant()) {
        MY.setEnPassant(victim, epFile);
        zobrist = PositionSide::zobrist_combine(MY, OP);
    }
}

void Position::set(Side My, Pi pi, PieceType ty, Square to) {
    const Side::_t Op{~My};

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
void Position::move(Pi pi, Square from, Square to) {
    const Side::_t Op{~My};

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));

    PieceType ty{MY.typeOf(pi)};
    assert (!ty.is(King));

    MY.move(pi, ty, from, to);
    set(My, pi, ty, to);

    MY.assertValid(pi);
}

template <Side::_t My>
void Position::makeKingMove(Square from, Square to) {
    const Side::_t Op{~My};

    Pi pi = TheKing;

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (MY.typeOf(pi).is(King));

    MY.moveKing(from, to);
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
    const Side::_t Op{~My};

    Square kingTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? C1:G1;
    Square rookTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? D1:F1;

    MY.castle(rook, rookFrom, rookTo, kingFrom, kingTo);

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
    const Side::_t Op{~My};

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
            move<My>(pi, from, _to);
            updateSliderAttacksKing<My>(MY.attacksTo(from, to, _to));
            updateSliderAttacks<Op>(OP.attacksTo(~from, ~to, ~_to));
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

        if (from.is(Rank2) && to.is(Rank4)) {
            setLegalEnPassant<My>(pi);
        }

    }

    MY.assertValid(pi);
}

template <Side::_t My>
void Position::makeMove(Square from, Square to) {
    const Side::_t Op{~My};

    //Assumes that the given move is valid and legal
    //En Passant capture encoded as the pawn captures the pawn
    //Castling move encoded as rook captures the king

    //clear en passant status from the previous move
    if (OP.hasEnPassant()) {
        OP.clearEnPassant();
        MY.clearEnPassants();
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

Move readMove(std::istream& in, const Position& pos, Color colorToMove) {
    in >> std::ws;
    if (in.eof()) { return Move::null(); }

    auto before_move = in.tellg();

    Square from{Square::Begin};
    Square to{Square::Begin};
    in >> from >> to;
    if (!in) { io::fail_pos(in, before_move); return Move::null(); }

    if (colorToMove.is(Black)) {
        from.flip();
        to.flip();
    }

    if (!pos.MY.isPieceOn(from) || (from == to)) { io::fail_pos(in, before_move); return Move::null(); }
    Pi pi{pos.MY.pieceOn(from)};

    //convert special moves (castling, promotion, ep) to the internal move format
    if (pos.MY.isPawn(pi)) {
        if (from.is(Rank5) && pos.OP.hasEnPassant()) {
            File epFile = pos.OP.enPassantFile();
            if (epFile.is(File(to))) {
                return Move::makeSpecial(from, Square(epFile, Rank5));
            }
        }
        else if (from.is(Rank7)) {
            PromoType promo{PromoType::Begin};
            if (in >> promo) {
                return Move(from, to, promo);
            }
            else { io::fail_pos(in, before_move); return Move::null(); }
        }
    }
    else if (pi.is(TheKing) && from.is(Rank1) && to.is(Rank1)) {
        if ( (pos.MY.castlingRooks() & pos.MY.piecesOn(to)).any() ) {
            //from Chess960 castling encoding
            return Move::makeCastling(to, from);
        }
        else if (from.is(E1) && to.is(G1) && pos.MY.isOccupied(A1)) {
            return Move::makeCastling(A1, E1);
        }
        else if (from.is(E1) && to.is(C1) && pos.MY.isOccupied(H1)) {
            return Move::makeCastling(H1, E1);
        }
    }
    return Move(from, to);
}

Move Position::createMove(Side My, Square from, Square to) const {
    const Side Op{~My};

    if ( MY.isPawn(MY.pieceOn(from)) ) {
        if ( from.is(Rank7) || (from.is(Rank5) && OP.hasEnPassant() && OP.enPassantFile().is(File(to))) ) {
            return Move::makeSpecial(from, to);
        }
    }
    else if (MY.kingSquare().is(to)) {
        return Move::makeCastling(from, to);
    }
    return Move(from, to);
}

#undef MY
#undef OP
#undef OCCUPIED
