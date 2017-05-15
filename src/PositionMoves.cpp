#include "PositionMoves.hpp"

#include "CastlingRules.hpp"
#include "PieceTypeAttack.hpp"

#define MY side[My]
#define OP side[Op]
#define OCCUPIED occupied[My]

template <Side::_t My>
void PositionMoves::generateEnPassantMoves() {
    constexpr Side Op{~My};

    assert (MY.hasEnPassant());
    assert (OP.hasEnPassant());

    File epFile = OP.enPassantFile();
    assert (MY.enPassantPawns() <= ( MY.pawns() & MY.attacksTo(Square(epFile, Rank6)) ));

    moves[Rank5] |= VectorPiRank(epFile) & VectorPiRank{MY.enPassantPawns()};
}

template <Side::_t My>
void PositionMoves::populateUnderpromotions() {
    constexpr Side Op{~My};

    //add underpromotions for each already generated legal queen promotion
    //TRICK: promoted piece type encoded inside pawn destination square rank
    VectorPiRank promotions = moves[::rankOf(Queen)] & VectorPiRank{MY.pawns()};
    moves[::rankOf(Rook)]   += promotions;
    moves[::rankOf(Bishop)] += promotions;
    moves[::rankOf(Knight)] += promotions;
}

template <Side::_t My>
void PositionMoves::generateKingMoves(Bb attackedSquares) {
    constexpr Side Op{~My};

    //TRICK: our attacks do not hide under attacked king shadow
    Bb kingMoves = ::pieceTypeAttack(King, MY.kingSquare()) % (MY.occupiedSquares() | attackedSquares);
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

    //after generic move generation
    //we need to correct moves of some pawns
    {
        //potential locations of the pawns where default generated moves should be corrected
        Bb badPawnsPlaces{checkLine << 8}; //simple pawn push over check line

        //the general case generates invalid diagonal moves to empty squares
        badPawnsPlaces |= (checkLine << 9) % Bb{FileA};
        badPawnsPlaces |= (checkLine << 7) % Bb{FileH};

        for (Square from : MY.occupiedByPawns() & badPawnsPlaces) {
            Pi pi{MY.pieceOn(from)};
            Rank rankTo = ::rankForward(Rank(from));

            Bb bb = (Bb{from.rankForward()} & checkLine) + (::pieceTypeAttack(Pawn, from) & checkFrom);
            moves.set(pi, rankTo, bb[rankTo]);
        }
    }

    //pawns double push over check line
    {
        Bb badPawnsPlaces{ Bb{Rank2} & (checkLine << 16) % (Bb{OCCUPIED} << 8)  };
        for (Square from : MY.occupiedByPawns() & badPawnsPlaces) {
            Pi pi{MY.pieceOn(from)};
            moves.add(pi, Rank4, File{from});
        }
    }

}

//exclude illegal moves due pin
template <Side::_t My>
void PositionMoves::excludePinnedMoves(VectorPiMask pinnerCandidates) {
    constexpr Side Op{~My};

    Square kingSquare = MY.kingSquare();

    for (Pi pi : pinnerCandidates) {
        Square pinFrom{~OP.squareOf(pi)};

        bool isPinnable = ::pieceTypeAttack(OP.typeOf(pi), pinFrom)[kingSquare];
        if (!isPinnable) {
            continue;
        }

        Bb pinRay = MY.pinRayFrom(pinFrom);
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
    constexpr Side Op{~My};

    VectorPiMask checkers{OP.attacksTo(~MY.kingSquare())};

    if (!checkers.isSingleton()) {
        //multiple (double) checker's case: no moves except king's ones are possible
        moves.clear();
    }
    else {
        //single checker case
        Pi checker{checkers.index()};
        Square checkFrom{~OP.squareOf(checker)};

        Bb checkLine = MY.pinRayFrom(checkFrom);

        //general case: check evasion moves of all pieces
        moves = MY.allAttacks() & (checkLine + checkFrom);

        //pawns moves are special case
        correctCheckEvasionsByPawns<My>(checkLine, checkFrom);

        excludePinnedMoves<My>(OP.sliders() % checkers);

        populateUnderpromotions<My>();

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
    constexpr Side Op{~My};

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
    excludePinnedMoves<My>(OP.sliders());

    populateUnderpromotions<My>();

    if (MY.hasEnPassant()) {
        generateEnPassantMoves<My>();
    }

    generateKingMoves<My>(attackedSquares);
}

void PositionMoves::limitMoves(std::istream& in, Color colorToMove) {
    MatrixPiBb searchMoves;
    index_t limit = 0;

    while (in) {
        auto before = in.tellg();

        Move m = readMove(in, colorToMove);
        if (in) {
            Square from{ m.from() };
            Square to{ m.to() } ;
            Pi pi{ MY.pieceOn(from) };

            if (moves.is(pi, to) && !searchMoves.is(pi, to)) {
                searchMoves.set(pi, to);
                ++limit;
                continue;
            }

        }

        io::fail_pos(in, before);
    }

    if (limit > 0) {
        moves = searchMoves;
        in.clear();
        return;
    }

    if (in.eof()) {
        io::fail_rewind(in);
    }
}

Color PositionMoves::setFen(std::istream& in) {
    Color colorToMove = Position::setFen(in);
    generateMoves<My>();
    return colorToMove;
}

void PositionMoves::makeMove(const Position& parent, Square from, Square to, Zobrist z) {
    Position::makeMove(parent, from, to, z);
    generateMoves<My>();
}

Color PositionMoves::makeMoves(std::istream& in, Color colorToMove) {
    while (in) {
        auto before = in.tellg();

        Move m = readMove(in, colorToMove);
        if (in) {
            Square from{ m.from() };
            Square to{ m.to() } ;
            Pi pi{ side[My].pieceOn(from) };

            if (is(pi, to)) {
                Position::makeMove(from, to);
                generateMoves<My>();
                colorToMove.flip();
                continue;
            }
        }

        io::fail_pos(in, before);
    }

    return colorToMove;
}

#undef MY
#undef OP
#undef OCCUPIED
