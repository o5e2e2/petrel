#include <utility>

#include "Position.hpp"
#include "BetweenSquares.hpp"
#include "CastlingRules.hpp"
#include "FenBoard.hpp"
#include "FenCastling.hpp"
#include "PieceTypeAttack.hpp"
#include "PositionMoves.hpp"
#include "OutsideSquares.hpp"

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
    Square kingSquare = MY.kingSquare();
    Bb pinners = ::outside(kingSquare, ~victimSquare) & ~OP.occupiedSquares();
    if (pinners.any()) {
        Square pinner = (kingSquare < ~victimSquare)? pinners.smallestOne() : pinners.largestOne();
        if (OP.pinnerCandidates()[OP.pieceOn(~pinner)] && (::between(kingSquare, pinner) & (OCCUPIED - ~victimSquare)).none()) {
            return false;
        }
    }

    setLegalEnPassant<Op>(victim);
    return true;
}

Zobrist Position::getZobrist() const {
    Zobrist oz = OP.getZobrist();

    if (OP.hasEnPassant()) {
        oz.setEnPassant(OP.enPassantSquare());
    }

    return Zobrist{MY.getZobrist(), oz};
}

void Position::setZobrist() {
    zobrist = getZobrist();
}

template <Side::_t My>
void Position::setLegalEnPassant(Pi pi) {
    constexpr Side Op{~My};

    assert (!MY.hasEnPassant());
    assert (!OP.hasEnPassant());

    Square to = MY.squareOf(pi);
    assert (to.is(Rank4));

    File epFile = File{to};
    Square _to(epFile, Rank3);

    Bb killers = ~OP.occupiedByPawns() & ::pieceTypeAttack(Pawn, _to);
    if (killers.none()) {
        return;
    }

    Square opKingSquare = ~OP.kingSquare();
    Square from(epFile, Rank2);

    Bb pinners = ::outside(opKingSquare, from) & MY.occupiedSquares();
    if (pinners.any()) {
        Square pinner = (opKingSquare < from)? pinners.smallestOne() : pinners.largestOne();
        if (MY.pinnerCandidates()[MY.pieceOn(pinner)] && (::between(opKingSquare, pinner) & OCCUPIED).none()) {
            assert ((MY.attacksTo(opKingSquare) % pi).any());
            return; //discovered check found
        }
    }
    assert ((MY.attacksTo(opKingSquare) % pi).none());

    for (Square _from : killers) {
        assert (_from.is(Rank4));

        pinners = ::outside(opKingSquare, _from) & (MY.occupiedSquares() - to);
        if (pinners.any()) {
            Square pinner = (opKingSquare < _from)? pinners.smallestOne() : pinners.largestOne();
            if (MY.pinnerCandidates()[MY.pieceOn(pinner)] && (::between(opKingSquare, pinner) & (OCCUPIED - _from + _to - to)).none()) {
                continue;
            }
        }
        OP.markEnPassant(OP.pieceOn(~_from));
    }

    if (OP.hasEnPassant()) {
        MY.setEnPassant(pi);
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

void Position::makeMove(const Position& parent, Square from, Square to) {
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
            Square _to(epFile, Rank3);

            Square opKingSquare = ~OP.kingSquare();

            Bb killers = ~OP.occupiedByPawns() & ::pieceTypeAttack(Pawn, _to);
            for (Square _from : killers) {
                assert (_from.is(Rank4));

                Bb pinners = ::outside(opKingSquare, _from) & MY.occupiedSquares();
                if (pinners.any()) {
                    Square pinner = (opKingSquare < _from)? pinners.smallestOne() : pinners.largestOne();
                    if (MY.pinnerCandidates()[MY.pieceOn(pinner)] && (::between(opKingSquare, pinner) & (OCCUPIED - _from + _to)).none()) {
                        continue;
                    }
                }

                pinners = ::outside(opKingSquare, from) & MY.occupiedSquares();
                if (pinners.any()) {
                    Square pinner = (opKingSquare < from)? pinners.smallestOne() : pinners.largestOne();
                    if (MY.pinnerCandidates()[MY.pieceOn(pinner)] && (::between(opKingSquare, pinner) & (OCCUPIED - from + to)).none()) {
                        return Zobrist{oz, mz}; //discovered check found
                    }
                }

                mz.setEnPassant(epFile);
                return Zobrist{oz, mz};
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

Move Position::operator() (Square move_from, Square move_to) const {
    if ( MY.isPawn(MY.pieceOn(move_from)) ) {
        if (move_from.is(Rank7)) {
            return Move::special(move_from, move_to);
        }
        if (move_from.is(Rank5) && move_to.is(Rank5)) {
            return Move::special(move_from, move_to);
        }
    }
    else if (MY.kingSquare().is(move_to)) {
        return Move::special(move_from, move_to);
    }

    return Move(move_from, move_to);
}

Move Position::operator() (std::istream& in, Color colorToMove) const {
    auto before = in.tellg();

    Square move_from{Square::Begin};
    Square move_to{Square::Begin};
    if (in >> std::ws >> move_from >> move_to) {
        if (colorToMove.is(Black)) {
            move_from.flip();
            move_to.flip();
        }

        if (MY.isPieceOn(move_from)) {
            Pi pi{MY.pieceOn(move_from)};

            //convert special moves (castling, promotion, ep) to the internal move format
            if (MY.isPawn(pi)) {
                if (move_from.is(Rank7)) {
                    PromoType promo{Queen};
                    in >> promo;
                    in.clear(); //promotion piece is optional
                    return Move::promotion(move_from, move_to, promo);
                }
                if (move_from.is(Rank5) && OP.hasEnPassant() && OP.enPassantFile().is(File(move_to))) {
                    return Move::enPassant(move_from, Square{File(move_to), Rank5});
                }
            }
            else if (pi.is(TheKing)) {
                if (MY.isPieceOn(move_to)) { //Chess960 castling encoding
                    return Move::castling(move_to, move_from);
                }
                if (move_from.is(E1) && move_to.is(G1) && MY.isPieceOn(H1)) {
                    return Move::castling(H1, E1);
                }
                if (move_from.is(E1) && move_to.is(C1) && MY.isPieceOn(A1)) {
                    return Move::castling(A1, E1);
                }
            }

            return Move(move_from, move_to);
        }
    }

    io::fail_pos(in, before);
    return Move{};
}

std::istream& Position::setEnPassant(std::istream& in, Color colorToMove) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    auto before = in.tellg();

    Square ep{Square::Begin};
    if (in >> ep) {
        if (ep.is(colorToMove.is(White)? Rank6 : Rank3) && setEnPassant(File{ep})) {
            return in;
        }
        else {
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
                if (setCastling(si, castlingSide)) {
                    continue;
                }
            }
            else {
                File file{File::Begin};
                if (file.from_char(c)) {
                    if (setCastling(si, file)) {
                        continue;
                    }
                }
            }
        }
        io::fail_char(in);
    }
    return in;
}

bool Position::setBoard(FenBoard& board, Color colorToMove) {
    *this = {0};

    //TRICK: kings should be placed before any opponent's non king pieces
    FOR_INDEX(Color, color) {
        if (board.pieces[color][King].empty()) {
            return false;
        }

        Side si(color.is(colorToMove)? My : Op);

        auto king = board.pieces[color][King].begin();

        if (drop(si, King, *king)) {
            board.pieces[color][King].erase(king);
        }
        else {
            return false;
        }

        assert (board.pieces[color][King].empty());
    }

    FOR_INDEX(Color, color) {
        Side si(color.is(colorToMove)? My : Op);

        FOR_INDEX(PieceType, ty) {
            while (!board.pieces[color][ty].empty()) {
                auto piece = board.pieces[color][ty].begin();

                if (drop(si, ty, *piece)) {
                    board.pieces[color][ty].erase(piece);
                }
                else {
                    return false;
                }
            }

        }
    }

    return setup();
}

std::istream& Position::setBoard(std::istream& in, Color* colorToMove) {
    FenBoard board;

    in >> board >> std::ws >> *colorToMove;

    if (in && !setBoard(board, *colorToMove)) {
        io::fail_char(in);
    }
    return in;
}

void Position::setFen(std::istream& in, Color& colorToMove) {
    setBoard(in, &colorToMove);
    setCastling(in, colorToMove);
    setEnPassant(in, colorToMove);
    setZobrist();

    if (in) {
        unsigned fifty, moves;
        in >> fifty >> moves;
        in.clear(); //ignore missing optional 'fifty' and 'moves' fen fields
    }
}

void Position::setStartpos(Color& colorToMove) {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    setFen(startpos, colorToMove);
}

void Position::fenEnPassant(std::ostream& out, Color colorToMove) const {
    if (side[Op].hasEnPassant()) {
        File epFile = side[Op].enPassantFile();
        out << (colorToMove.is(White)? Square(epFile, Rank6) : Square(epFile, Rank3));
    }
    else {
        out << '-';
    }
}

void Position::fenBoard(std::ostream& out, const PositionSide& white, const PositionSide& black) {
    FOR_INDEX(Rank, rank) {
        index_t blank_squares = 0;

        FOR_INDEX(File, file) {
            Square sq(file,rank);

            if (white.isOccupied(sq)) {
                if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                out << static_cast<io::char_type>(std::toupper( white.typeOf(sq).to_char() ));
            }
            else if (black.isOccupied(~sq)) {
                if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                out << black.typeOf(~sq).to_char();
            }
            else {
                ++blank_squares;
            }

        }

        if (blank_squares != 0) { out << blank_squares; }
        if (rank != Rank1) { out << '/'; }
    }
}

void Position::fen(std::ostream& out, Color colorToMove, ChessVariant chessVariant) const {
    const PositionSide& white = side[colorToMove.is(White)? My : Op];
    const PositionSide& black = side[colorToMove.is(Black)? My : Op];

    fenBoard(out, white, black);
    out << ' ' << colorToMove;
    out << ' ' << FenCastling(white, black, chessVariant);
    out << ' ';
    fenEnPassant(out, colorToMove);
}

#undef MY
#undef OP
#undef OCCUPIED
