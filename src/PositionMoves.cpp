#include "PositionMoves.hpp"

#include "CastlingRules.hpp"
#include "PieceTypeAttack.hpp"

#define MY (this->ps)[My]
#define OP (this->ps)[Op]
#define OCCUPIED (this->ps)[My].occupied()

template <Side::_t My>
void PositionMoves::generateEnPassantMoves() {
    constexpr Side Op{~My};

    assert (MY.hasEnPassant());
    assert (OP.hasEnPassant());

    File epFile = OP.enPassantFile();
    assert (MY.enPassantPawns() <= ( MY.pawns() & MY.attackersTo(Square{epFile, Rank6}) ));

    moves[Rank5] |= VectorPiRank(epFile) & VectorPiRank{MY.enPassantPawns()};
}

template <Side::_t My>
void PositionMoves::populateUnderpromotions() {
    constexpr Side Op{~My};

    //add underpromotions for each already generated legal queen promotion
    //TRICK: promoted piece type encoded inside pawn destination square rank
    VectorPiRank promotionFiles = moves[Rank8] & VectorPiRank{MY.pawns()};
    moves[::rankOf(Rook)]   += promotionFiles;
    moves[::rankOf(Bishop)] += promotionFiles;
    moves[::rankOf(Knight)] += promotionFiles;
}

template <Side::_t My>
void PositionMoves::generateLegalKingMoves(Bb attackedSquares) {
    constexpr Side Op{~My};

    //TRICK: our attacks do not hide under attacked king shadow
    Bb kingMoves = ::attacksFrom(King, MY.kingSquare()) % (MY.sideSquares() | attackedSquares);
    moves.set(TheKing, kingMoves);
}

template <Side::_t My>
void PositionMoves::generateCastlingMoves(Bb attackedSquares) {
    constexpr Side Op{~My};

    for (Pi pi : MY.castlingRooks()) {
        if ( ::castlingRules.isLegal(MY.kingSquare(), MY.squareOf(pi), OCCUPIED, attackedSquares) ) {
            //castling encoded as the rook moves over the own king square
            moves.add(pi, MY.kingSquare());
        }
    }
}

template <Side::_t My>
void PositionMoves::generatePawnMoves() {
    constexpr Side Op{~My};

    for (Pi pi : MY.pawns()) {
        Square from{ MY.squareOf(pi) };

        Rank rankTo{ ::rankForward(Rank{from}) };
        BitRank fileTo{File{from}};

        //push to free square
        fileTo %= OCCUPIED[rankTo];

        //double push
        if ((rankTo.is(Rank3)) && (fileTo % OCCUPIED[Rank4]).any()) {
            moves.set(pi, Rank4, fileTo);
        }

        //remove "captures" of free squares from default generated moves
        fileTo += moves[rankTo][pi] & OCCUPIED[rankTo];

        moves.set(pi, rankTo, fileTo);
    }
}

template <Side::_t My>
void PositionMoves::correctCheckEvasionsByPawns(Bb checkLine, Square checkFrom) {
    constexpr Side Op{~My};

    //simple pawn push over check line
    Bb potencialEvasions = checkLine << 8;

    //the general case generates invalid diagonal moves to empty squares
    Bb invalidDiagonalMoves = (checkLine << 9) % Bb{FileA} | (checkLine << 7) % Bb{FileH};

    Bb affectedPawns = MY.pawnsSquares() & (potencialEvasions | invalidDiagonalMoves);
    for (Square from : affectedPawns) {
        Rank rankTo = ::rankForward(Rank{from});
        Bb bb = (Bb{from.rankForward()} & checkLine) + (::attacksFrom(Pawn, from) & checkFrom);
        moves.set(MY.pieceOn(from), rankTo, bb[rankTo]);
    }

    //pawns double push over check line
    Bb pawnJumpEvasions = MY.pawnsSquares() & Bb{Rank2} & (checkLine << 16) % (OCCUPIED << 8);
    for (Square from : pawnJumpEvasions) {
        moves.add(MY.pieceOn(from), Rank4, File{from});
    }

}

//exclude illegal moves due pin
template <Side::_t My>
void PositionMoves::excludePinnedMoves(VectorPiMask pinnerCandidates) {
    constexpr Side Op{~My};

    for (Pi pi : pinnerCandidates) {
        Square pinFrom{~OP.squareOf(pi)};

        assert (::attacksFrom(OP.typeOf(pi), pinFrom).has(MY.kingSquare()));

        const Bb& pinLine = ::between(MY.kingSquare(), pinFrom);
        Bb betweenPieces = pinLine & OCCUPIED;
        assert (betweenPieces.any());

        if (betweenPieces.isSingleton() && (betweenPieces & MY.sideSquares()).any()) {
            //we discovered a true pinned piece
            Pi pinned = MY.pieceOn(betweenPieces.index());

            //exclude moves except moves over the pin line
            moves.filter(pinned, pinLine + pinFrom);
        }
    }
}

template <Side::_t My>
void PositionMoves::generateCheckEvasions(Bb attackedSquares) {
    constexpr Side Op{~My};

    VectorPiMask checkers = OP.checkers();

    if (!checkers.isSingleton()) {
        //multiple (double) checker's case: no moves except king's ones are possible
        moves.clear();
    }
    else {
        //single checker case
        Pi checker = checkers.index();
        Square checkFrom{~OP.squareOf(checker)};

        const Bb& checkLine = ::between(MY.kingSquare(), checkFrom);

        //general case: check evasion moves of all pieces
        moves = MY.attacksMatrix() & (checkLine + checkFrom);

        //pawns moves are special case
        correctCheckEvasionsByPawns<My>(checkLine, checkFrom);

        excludePinnedMoves<My>(OP.pinners() % checkers);

        populateUnderpromotions<My>();

        if (MY.hasEnPassant()) {
            assert (OP.enPassantPawns() == checkers);
            generateEnPassantMoves<My>();
        }
    }

    generateLegalKingMoves<My>(attackedSquares);
}

//generate all legal moves from the current position for the current side to move
template <Side::_t My>
void PositionMoves::generateMoves() {
    constexpr Side Op{~My};

    //squares attacked by all opponent pieces
    Bb attackedSquares = ~OP.attacksMatrix().gather();

    bool inCheck = attackedSquares.has(MY.kingSquare());
    if (inCheck) {
        generateCheckEvasions<My>(attackedSquares);
        return;
    }

    //the most general case: captures and non captures for all pieces
    moves = MY.attacksMatrix() % MY.sideSquares();

    //pawns moves treated separately
    generatePawnMoves<My>();

    generateCastlingMoves<My>(attackedSquares);

    //TRICK: castling encoded as a rook move, so we implicitly cover the case of pinned castling in Chess960
    excludePinnedMoves<My>(OP.pinners());

    populateUnderpromotions<My>();

    if (MY.hasEnPassant()) {
        generateEnPassantMoves<My>();
    }

    generateLegalKingMoves<My>(attackedSquares);
}

void PositionMoves::setMoves() {
    generateMoves<My>();
    movesCount = moves.count();

    if (movesCount == 0) {
        bool inCheck = OP.checkers().any();
        staticEval = inCheck ? Score::Checkmated : Score::Draw;
        return;
    }

    staticEval = evaluate();
}

bool PositionMoves::isLegalMove(Square from, Square to) const {
    auto pi = MY.pieceOn(from);
    return moves.has(pi, to);
}

void PositionMoves::playMove(Square from, Square to) {
    setZobrist(*this, from, to);
    Position::playMove(from, to);
    setMoves();
    assert (zobrist == generateZobrist());
}

void PositionMoves::playMove(const Position& parent, Square from, Square to, Zobrist z) {
    zobrist = z;
    Position::playMove(parent, from, to);
    setMoves();
    assert (zobrist == Zobrist{0} || zobrist == generateZobrist());
}

Zobrist PositionMoves::generateZobrist() const {
    auto mz = MY.generateZobrist();
    auto oz = OP.generateZobrist();

    if (OP.hasEnPassant()) {
        oz.setEnPassant(OP.enPassantSquare());
    }

    return Zobrist(mz, oz);
}

Zobrist PositionMoves::createZobrist(Square from, Square to) const {
    Zobrist mz{zobrist};
    Zobrist oz{0};

    if (OP.hasEnPassant()) {
        oz.clearEnPassant(OP.enPassantSquare());
    }

    Pi pi = MY.pieceOn(from);
    PieceType ty {MY.typeOf(pi)};

    if (ty.is(Pawn)) {
        if (from.on(Rank7)) {
            Square promotedTo = Square{File{to}, Rank8};
            mz.clear(Pawn, from);
            mz.drop(static_cast<PieceType>(PromoType{to}), promotedTo);

            if (OP.isOccupied(~promotedTo)) {
                Pi victim = OP.pieceOn(~promotedTo);

                if (OP.isCastling(victim)) {
                    oz.clearCastling(~promotedTo);
                }
                oz.clear(OP.typeOf(victim), ~promotedTo);
            }
            return Zobrist{oz, mz};
        }

        if (from.on(Rank2) && to.on(Rank4)) {
            mz.move(ty, from, to);

            File file = File{from};
            Square ep(file, Rank3);

            Bb killers = ~OP.pawnsSquares() & ::attacksFrom(Pawn, ep);
            if (killers.any() && !MY.isPinned(OCCUPIED - from + ep)) {
                for (Square killer : killers) {
                    assert (killer.on(Rank4));

                    if (!MY.isPinned(OCCUPIED - killer + ep)) {
                        mz.setEnPassant(file);
                        return Zobrist{oz, mz};
                    }
                }
            }
            return Zobrist{oz, mz};
        }

        if (from.on(Rank5) && to.on(Rank5)) {
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
        Square kingTo = CastlingRules::castlingKingTo(kingFrom, rookFrom);
        Square rookTo = CastlingRules::castlingRookTo(kingFrom, rookFrom);

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
        Pi victim = OP.pieceOn(~to);
        oz.clear(OP.typeOf(victim), ~to);

        if (OP.isCastling(victim)) {
            oz.clearCastling(~to);
        }
    }

    mz.move(ty, from, to);
    return Zobrist{oz, mz};
}

#undef MY
#undef OP
#undef OCCUPIED
