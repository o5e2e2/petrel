#include "PositionMoves.hpp"
#include "Position.hpp"

#include "CastlingRules.hpp"
#include "PieceTypeAttack.hpp"

#define MY pos.side[My]
#define OP pos.side[Op]
#define OCCUPIED pos.occupied[My]

const PositionSide& PositionMoves::side(Side si) const {
    return pos.side[si];
}

template <Side::_t My>
void PositionMoves::generateEnPassantMoves() {
    const Side::_t Op{~My};

    assert (MY.hasEnPassant());
    assert (OP.hasEnPassant());

    File ep{OP.enPassantFile()};
    assert ((MY.pawns() & MY.attacksTo(Square(ep, Rank6)) & MY.enPassantPawns()) == MY.enPassantPawns());

    moves[Rank5] |= VectorPiRank{ep} & VectorPiRank{MY.enPassantPawns()};
}

template <Side::_t My>
void PositionMoves::generateUnderpromotions() {
    const Side::_t Op{~My};

    //TRICK: promoted piece type encoded inside pawn destination square rank
    VectorPiRank promotions = moves[Rank8] & VectorPiRank{MY.pawns()};

    static_assert (static_cast<Rank::_t>(Queen) == Rank8, "invalid promotion piece encoding");
    moves[static_cast<Rank::_t>(Rook)] += promotions;
    moves[static_cast<Rank::_t>(Bishop)] += promotions;
    moves[static_cast<Rank::_t>(Knight)] += promotions;
}

template <Side::_t My>
void PositionMoves::generateKingMoves(Bb attackedSquares) {
    const Side::_t Op{~My};

    //TRICK: our attacks do not hide under attacked king shadow
    Bb kingMoves = ::pieceTypeAttack(King, MY.kingSquare()) % (MY.occupiedSquares() | attackedSquares);
    moves.set(TheKing, kingMoves);
}

template <Side::_t My>
void PositionMoves::generateCastlingMoves(Bb attackedSquares) {
    const Side::_t Op{~My};

    for (Pi pi : MY.castlingRooks()) {
        if ( ::castlingRules.isLegal(MY.castlingSideOf(pi), OCCUPIED, attackedSquares) ) {
            //castling encoded as the rook moves over the own king square
            moves.add(pi, MY.kingSquare());
        }
    }
}

template <Side::_t My>
void PositionMoves::generatePawnMoves() {
    const Side::_t Op{~My};

    for (Pi pi : MY.pawns()) {
        Square from{ MY.squareOf(pi) };
        Rank rankTo{ up(Rank{from}) };
        BitRank fileTo{File{from}};

        //push to free square
        fileTo %= OCCUPIED[rankTo];

        //double push
        if ((rankTo == Rank3) && (fileTo % OCCUPIED[Rank4]).any()) {
            moves.set(pi, Rank4, fileTo);
        }

        //remove "captures" of free squares from default generated moves
        fileTo += moves[rankTo][pi] & OCCUPIED[rankTo];

        moves.set(pi, rankTo, fileTo);
    }
}

template <Side::_t My>
void PositionMoves::correctCheckEvasionsByPawns(Bb checkLine, Square checkFrom) {
    const Side::_t Op{~My};

    //after generic move generation
    //we need to correct moves of some pawns
    {
        //potential locations of the pawns which default generated moves should be corrected
        Bb badPawnsPlaces{checkLine << 8}; //simple pawn push over check line

        //the general case generates invalid diagonal moves to empty squares
        badPawnsPlaces |= (checkLine << 9) % Bb{FileA};
        badPawnsPlaces |= (checkLine << 7) % Bb{FileH};

        for (Square from : MY.occupiedByPawns() & badPawnsPlaces) {
            Pi pi{MY.pieceOn(from)};

            Bb b{Bb{from.rankUp()} % OCCUPIED & checkLine};
            b += ::pieceTypeAttack(Pawn, from) & checkFrom;

            Rank rank{from.rankUp()};
            moves.set(pi, rank, b[rank]);
        }
    }

    //pawns double push over check line
    {
        Bb badPawnsPlaces{ (checkLine<<16) % (Bb{OCCUPIED}<<8) & Bb{Rank2} };
        for (Square from : MY.occupiedByPawns() & badPawnsPlaces) {
            Pi pi{MY.pieceOn(from)};
            moves.add(pi, Rank4, File{from});
        }
    }

}

//exclude illegal moves due pin
template <Side::_t My>
void PositionMoves::excludePinnedMoves(VectorPiMask pinnerCandidates) {
    const Side::_t Op{~My};

    for (Pi pi : pinnerCandidates) {
        Square pinFrom{~OP.squareOf(pi)};
        auto pinRay = MY.pinRayFrom(pinFrom);

        Bb betweenPieces{pinRay & OCCUPIED};
        assert (betweenPieces.any());

        if (betweenPieces.isSingleton() && (betweenPieces & MY.occupiedSquares()).any()) {
            //we discovered a true pinned piece
            Pi pinned{MY.pieceOn(betweenPieces.index())};

            //exclude moves except moves over the pin line
            moves.filter(pinned, pinRay + pinFrom);
        }
    }
}

template <Side::_t My>
void PositionMoves::generateCheckEvasions(Bb attackedSquares) {
    const Side::_t Op{~My};

    VectorPiMask checkers{OP.attacksTo(~MY.kingSquare())};

    if (!checkers.isSingleton()) {
        //multiple (double) checker's case: no moves except king's ones are possible
        moves.clear();
    }
    else {
        //single checker case
        Pi checker{checkers.index()};
        Square checkFrom{~OP.squareOf(checker)};

        auto checkLine = MY.pinRayFrom(checkFrom);

        //general case: check evasion moves of all pieces
        moves = MY.allAttacks() & (checkLine + checkFrom);

        //pawns moves are special case
        correctCheckEvasionsByPawns<My>(checkLine, checkFrom);

        excludePinnedMoves<My>(OP.pinnerCandidates() % checkers);

        generateUnderpromotions<My>();

        if (MY.hasEnPassant()) {
            assert (OP.enPassantPawns() == checkers);
            generateEnPassantMoves<My>();
        }
    }

    generateKingMoves<My>(attackedSquares);
}

//generate all legal moves from the current position for the current side to move
template <Side::_t My>
void PositionMoves::generateMoves() {
    const Side::_t Op{~My};

    //squares attacked by opponent pieces
    Bb attackedSquares = ~OP.allAttacks().gather();

    if ( attackedSquares[MY.kingSquare()] ) {
        generateCheckEvasions<My>(attackedSquares);
        return;
    }

    //the most general case: captures and non captures for all pieces
    moves = MY.allAttacks() % MY.occupiedSquares();

    //pawns moves treated separately
    generatePawnMoves<My>();

    generateCastlingMoves<My>(attackedSquares);

    //TRICK: castling encoded as a rook move, so we implicitly cover the case of pinned castling in Chess960
    excludePinnedMoves<My>(OP.pinnerCandidates());

    //add underpromotions for each already generated legal queen promotion
    generateUnderpromotions<My>();

    if (MY.hasEnPassant()) {
        generateEnPassantMoves<My>();
    }

    generateKingMoves<My>(attackedSquares);
}

void PositionMoves::limitMoves(std::istream& in, MatrixPiBb& searchmoves, Color color) const {
    while (in) {
        Move m { readMove(in, pos, color) };
        if (m.isNull()) { break; }

        Square from{m.from()};
        if (MY.isPieceOn(from)) {
            Pi pi{MY.pieceOn(from)};
            Square to{m.to()};

            if (moves.is(pi, to)) {
                searchmoves.set(pi, to);
            }
        }
        else {
            break;
        }
    }

}

void PositionMoves::makeMoves(std::istream& in, Color& colorToMove) {
    while (in) {
        auto before_current_move = in.tellg(); //save the streampos for error reporting

        Move m{ readMove(in, pos, colorToMove) };

        if (m.isNull()) {
            if (!in.eof()) { io::fail_pos(in, before_current_move); }
            break;
        }

        if (!moves.is(MY.pieceOn(m.from()), m.to())) {
            //detected an illegal move
            io::fail_pos(in, before_current_move);
            break;
        }

        colorToMove.flip();
        const_cast<Position&>(pos).swapSides();
        const_cast<Position&>(pos).makeMove<Op>(m.from(), m.to());
        generateMoves<My>();
    }
}

#undef MY
#undef OP
#undef OCCUPIED
