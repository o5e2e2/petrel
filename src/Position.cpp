#include <utility>

#include "Position.hpp"
#include "BetweenSquares.hpp"
#include "CastlingRules.hpp"
#include "FenBoard.hpp"
#include "PieceTypeAttack.hpp"
#include "OutsideSquares.hpp"

#define MY side[My]
#define OP side[Op]
#define OCCUPIED side[My].occupied()

bool Position::setup() {
    setStage<My>();
    setStage<Op>();
    updateSliderAttacksKing<Op>(OP.alivePieces());
    setSliderAttacks<My>(MY.alivePieces());
    return MY.attacksTo(~OP.kingSquare()).none(); //not in check
}

bool Position::drop(Side My, PieceType ty, Square to) {
    const Side Op{~My};

    if (OCCUPIED[to]) { return false; }

    if (ty.is(King)) {
        //TRICK: king should be dropped before any opponents non-king pieces
        MY.drop(TheKing, King, to);
        return true;
    }

    if (ty.is(Pawn) && (to.is(Rank1) || to.is(Rank8))) {
        return false;
    }

    Pi pi{ (MY.alivePieces() | Pi{TheKing}).seekVacant() };
    MY.drop(pi, ty, to);
    return true;
}

template <Side::_t My>
bool Position::isPinned(Square opPinned, Bb myOccupied, Bb allOccupied) const {
    constexpr Side Op{~My};

    Square opKingSquare = ~OP.kingSquare();

    Bb pinners = ::outside(opKingSquare, opPinned) & myOccupied;
    if (pinners.none()) {
        return false;
    }

    Square pinner = (opKingSquare < opPinned)? pinners.smallestOne() : pinners.largestOne();
    return MY.canBeAttacked(pinner, opKingSquare)
        && (::between(opKingSquare, pinner) & (allOccupied - opPinned)).none();
}

template <Side::_t My>
void Position::updateSliderAttacksKing(VectorPiMask affected) {
    constexpr Side Op{~My};

    //sync occupiedBb between sides
    MY.setOccupied(MY.occupiedSquares() + ~OP.occupiedSquares());
    OP.setOccupied(~OCCUPIED);

    //TRICK: attacks calculated without opponent's king for implicit out of check king's moves generation
    MY.setSliderAttacks(affected & MY.sliders(), OCCUPIED - ~OP.kingSquare());
}

template <Side::_t My>
void Position::setSliderAttacks(VectorPiMask affected) {
    constexpr Side Op{~My};

    MY.setSliderAttacks(affected & MY.sliders(), OCCUPIED);

    //king cannot be in check
    assert (MY.attacksTo(~OP.kingSquare()).none());
}

template <Side::_t My>
void Position::setStage() {
    constexpr Side Op{~My};
    MY.setStage(OP.getStage());
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

    if (isPinned<My>(from, MY.occupiedSquares(), OCCUPIED + from)) {
        assert ((MY.attacksTo(~OP.kingSquare()) % pi).any());
        return; //discovered check found
    }
    assert ((MY.attacksTo(~OP.kingSquare()) % pi).none());

    for (Square killer : killers) {
        assert (killer.is(Rank4));

        if (!isPinned<My>(killer, MY.occupiedSquares() - to, OCCUPIED + ep - to)) {
            OP.markEnPassant(OP.pieceOn(~killer));
            MY.setEnPassant(pi);
        }
    }
}

template <Side::_t My>
void Position::capture(Square from) {
    constexpr Side Op{~My};
    MY.capture(from);
    setStage<Op>();
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
void Position::move(Pi pi, Square from, Square to) {
    constexpr Side Op{~My};

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (!MY.isPawn(pi));

    PieceType ty{MY.typeOf(pi)};
    assert (!ty.is(King));

    MY.move(pi, ty, from, to);
}

template <Side::_t My>
void Position::makeKingMove(Square from, Square to) {
    constexpr Side Op{~My};

    Pi pi = TheKing;

    MY.assertValid(pi);
    assert (MY.squareOf(pi).is(from));
    assert (MY.typeOf(pi).is(King));

    MY.moveKing(from, to);

    if (OP.isOccupied(~to)) {
        //capture
        capture<Op>(~to);
        updateSliderAttacksKing<My>(MY.attacksTo(from));
        return;
    }

    //non-capture
    updateSliderAttacksKing<My>(MY.attacksTo(from, to));
}

template <Side::_t My>
void Position::makeCastling(Pi rook, Square rookFrom, Square kingFrom) {
    constexpr Side Op{~My};

    Square kingTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? C1 : G1;
    Square rookTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? D1 : F1;

    MY.castle(rook, rookFrom, rookTo, kingFrom, kingTo);

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
        PromoType ty{to};
        Square promotedTo = Square(File(to), Rank8);

        MY.promote(pi, ty, from, promotedTo);
        setStage<Op>();

        if (OP.isOccupied(~promotedTo)) {
            //promotion with capture
            capture<Op>(~promotedTo);
            updateSliderAttacksKing<My>(MY.attacksTo(from) | pi);
            setSliderAttacks<Op>(OP.attacksTo(~from));
            return;
        }

        //promotion withoout capture
        updateSliderAttacksKing<My>(MY.attacksTo(from, promotedTo) | pi);
        setSliderAttacks<Op>(OP.attacksTo(~from, ~promotedTo));
        return;
    }

    if (OP.isOccupied(~to)) {
        capture<Op>(~to);

        if (from.is(Rank5) && to.is(Rank5)) {
            //en passant move
            Square ep(File{to}, Rank6);
            movePawn<My>(pi, from, ep);
            updateSliderAttacksKing<My>(MY.attacksTo(from, to, ep));
            setSliderAttacks<Op>(OP.attacksTo(~from, ~to, ~ep));
            return;
        }

        //simple pawn capture
        movePawn<My>(pi, from, to);
        updateSliderAttacksKing<My>(MY.attacksTo(from));
        setSliderAttacks<Op>(OP.attacksTo(~from));
        return;
    }

    //simple pawn move
    movePawn<My>(pi, from, to);

    updateSliderAttacksKing<My>(MY.attacksTo(from, to));
    setSliderAttacks<Op>(OP.attacksTo(~from, ~to));

    if (from.is(Rank2) && to.is(Rank4)) {
        setLegalEnPassant<My>(pi);
    }
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
        capture<Op>(~to);
        move<My>(pi, from, to);
        updateSliderAttacksKing<My>(MY.attacksTo(from) | pi);
        setSliderAttacks<Op>(OP.attacksTo(~from));
    }
    else {
        if (MY.kingSquare().is(to)) {
            makeCastling<My>(pi, from, to);
        }
        else {
            //simple non-pawn move
            move<My>(pi, from, to);
            updateSliderAttacksKing<My>(MY.attacksTo(from, to));
            setSliderAttacks<Op>(OP.attacksTo(~from, ~to));
        }
    }
}
void Position::makeMove(const Position& parent, Square from, Square to, Zobrist z) {
    zobrist = z;

    assert (this != &parent);
    MY = parent.OP;
    OP = parent.MY;

    //current position flipped its sides relative to parent, so we make the move inplace for the Op
    makeMove<Op>(from, to);
}

void Position::playMove(Square from, Square to) {
    zobrist = makeZobrist(from, to);
    PositionSide::swap(MY, OP);

    //the position just swapped its sides, so we make the move for the Op
    makeMove<Op>(from, to);
}

Zobrist Position::generateZobrist() const {
    auto mz = MY.generateZobrist();
    auto oz = OP.generateZobrist();

    if (OP.hasEnPassant()) {
        oz.setEnPassant(OP.enPassantSquare());
    }

    return Zobrist(mz, oz);
}

Zobrist Position::makeZobrist(Square from, Square to) const {
    Zobrist mz = zobrist;
    Zobrist oz = {};

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
            if (killers.none()) {
                return Zobrist{oz, mz};
            }

            if (isPinned<My>(from, MY.occupiedSquares(), OCCUPIED + ep)) {
                return Zobrist{oz, mz}; //discovered check found
            }

            for (Square killer : killers) {
                assert (killer.is(Rank4));

                if (!isPinned<My>(killer, MY.occupiedSquares(), OCCUPIED + ep)) {
                    mz.setEnPassant(file);
                    return Zobrist{oz, mz};
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
        Square kingTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? C1 : G1;
        Square rookTo = CastlingRules::castlingSide(kingFrom, rookFrom).is(QueenSide)? D1 : F1;

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

Move Position::createMove(Square moveFrom, Square moveTo) const {
    if (MY.kingSquare().is(moveTo)) {
        return Move::castling(moveFrom, moveTo);
    }

    if ( MY.isPawn(MY.pieceOn(moveFrom)) ) {
        if (moveFrom.is(Rank7)) {
            return Move::promotion(moveFrom, moveTo);
        }
        if (moveFrom.is(Rank5) && moveTo.is(Rank5)) {
            return Move::enPassant(moveFrom, moveTo);
        }
    }
    return Move(moveFrom, moveTo);
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
    if (isPinned<Op>(victimSquare, OP.occupiedSquares(), OP.occupied())) {
        return false;
    }

    setLegalEnPassant<Op>(victim);
    return true;
}

std::istream& Position::setEnPassant(std::istream& in, Color colorToMove) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    auto before = in.tellg();

    Square ep{Square::Begin};
    if (in >> ep) {
        if (!ep.is(colorToMove.is(White)? Rank6 : Rank3) || !setEnPassant(File{ep})) {
            io::fail_pos(in, before);
        }
    }
    return in;
}

std::istream& Position::setCastling(std::istream& in, Color colorToMove) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    for (io::char_type c; in.get(c) && !std::isblank(c); ) {
        if (std::isalpha(c)) {
            Color color(std::isupper(c)? White : Black);
            Side si(color.is(colorToMove)? My : Op);

            c = static_cast<io::char_type>(std::tolower(c));

            CastlingSide castlingSide{CastlingSide::Begin};
            if (castlingSide.from_char(c)) {
                if (side[si].setCastling(castlingSide)) {
                    continue;
                }
            }
            else {
                File file{File::Begin};
                if (file.from_char(c)) {
                    if (side[si].setCastling(file)) {
                        continue;
                    }
                }
            }
        }
        io::fail_char(in);
    }
    return in;
}

Color Position::setBoard(std::istream& in) {
    Color colorToMove;
    FenBoard board;

    in >> board >> std::ws >> colorToMove;

    if (in && !board.setPosition(*this, colorToMove)) {
        io::fail_char(in);
    }

    return colorToMove;
}

Color Position::setupFromFen(std::istream& in) {
    Color colorToMove = setBoard(in);
    setCastling(in, colorToMove);
    setEnPassant(in, colorToMove);
    zobrist = generateZobrist();
    return colorToMove;
}

#undef MY
#undef OP
#undef OCCUPIED
