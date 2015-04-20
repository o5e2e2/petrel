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

    File epFile = OP.enPassantFile();
    assert (MY.enPassantPawns() <= ( MY.pawns() & MY.attacksTo(Square(epFile, Rank6)) ));

    moves[Rank5] |= VectorPiRank(epFile) & VectorPiRank{MY.enPassantPawns()};
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
        if ( ::castlingRules.isLegal(MY.kingSquare(), MY.squareOf(pi), OCCUPIED, attackedSquares) ) {
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

        Bb checkLine = MY.pinRayFrom(checkFrom);

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

std::istream& PositionMoves::readMove(std::istream& in, Move& move, Color colorToMove) const {
    auto before_move = in.tellg();

    Square from{Square::Begin};
    Square to{Square::Begin};

    in >> std::ws >> from >> to;
    if (!in) { goto fail; }

    if (colorToMove.is(Black)) {
        from.flip();
        to.flip();
    }

    if (MY.isPieceOn(from) && (from != to)) {
        Pi pi{MY.pieceOn(from)};

        //convert special moves (castling, promotion, ep) to the internal move format
        if (pi.is(TheKing) && to.is(Rank1)) {
            if (from.is(E1) && to.is(G1)) {
                if (MY.isPieceOn(A1) && (MY.castlingRooks() & MY.piecesOn(A1)).any()) {
                    move = Move::castlingMove(A1, E1);
                    return in;
                }
                else { goto fail; }
            }
            else if (from.is(E1) && to.is(C1)) {
                if (MY.isPieceOn(H1) && (MY.castlingRooks() & MY.piecesOn(H1)).any()) {
                    move = Move::castlingMove(H1, E1);
                    return in;
                }
                else { goto fail; }
            }
            else if (MY.isPieceOn(to)) { //Chess960 castling encoding
                if ((MY.castlingRooks() & MY.piecesOn(to)).any()) {
                    move = Move::castlingMove(to, from);
                    return in;
                }
                else { goto fail; }
            }
            //else generic move
        }
        else if (MY.isPawn(pi)) {
            if (from.is(Rank7)) {
                PromoType promo{PromoType::Begin};
                if (in >> promo) {
                    move = Move(from, to, promo);
                    return in;
                }
                else { goto fail; }
            }
            else if (from.is(Rank5) && OP.hasEnPassant() && OP.enPassantFile().is(File(to))) {
                move = Move::enPassantMove(from, Square(File(to), Rank5));
                return in;
            }
            //else generic move
        }

        move = Move(from, to);
        return in;
    }

fail:
    move = Move::nullMove();
    return io::fail_pos(in, before_move);
}

void PositionMoves::limitMoves(std::istream& in, Color color) {
    MatrixPiBb searchMoves;
    index_t limit = 0;

    while (in) {
        auto before_move = in.tellg();

        Move move;
        if (readMove(in, move, color)) {
            Pi pi{ MY.pieceOn(move.from()) };
            Square to{ move.to() };

            if (moves.is(pi, to) && !searchMoves.is(pi, to)) {
                searchMoves.set(pi, to);
                ++limit;
                continue;
            }

        }

        io::fail_pos(in, before_move);
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

void PositionMoves::makeMoves(std::istream& in, Position& pos, Color& colorToMove) {
    PositionMoves pm(pos);

    while (in) {
        auto before_move = in.tellg();

        Move move;
        if (pm.readMove(in, move, colorToMove)) {
            Square from{ move.from() };
            Square to{ move.to() } ;
            Pi pi{ MY.pieceOn(move.from()) };

            pm.generateMoves();
            if (pm.moves.is(pi, to)) {
                pos.makeMove(pos, from, to);
                colorToMove.flip();
                continue;
            }
        }

        io::fail_pos(in, before_move);
    }
}

#undef MY
#undef OP
#undef OCCUPIED
