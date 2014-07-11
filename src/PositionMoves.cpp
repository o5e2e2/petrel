#include "PositionMoves.hpp"
#include "Position.hpp"

#include "CastlingRules.hpp"
#include "Node.hpp"
#include "PieceTypeAttack.hpp"

#define MY pos.side[My]
#define OP pos.side[Op]

PositionMoves::PositionMoves (const Position& p) : pos(p) { generateMoves<My>(); }

const PositionSide& PositionMoves::side(Side si) const {
    return pos.side[si];
}

template <Side::_t My>
void PositionMoves::generateEnPassantMoves() {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    assert (MY.enPassants().any());

    Square ep{~OP.enPassantSquare()};
    assert (ep.is<Rank5>());
    assert ((MY.pawns() & MY.attacksTo(ep.rankUp()) & MY.enPassants()) == MY.enPassants());

    moves[Rank5] |= VectorPiRank{File{ep}} & VectorPiRank{MY.enPassants()};
}

template <Side::_t My>
void PositionMoves::generateUnderpromotions() {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    assert (static_cast<Rank::_t>(Queen) == Rank8);
    VectorPiRank promotions = moves[Rank8] & VectorPiRank{MY.pawns()};

    moves[static_cast<Rank::_t>(Rook)] |= promotions;
    moves[static_cast<Rank::_t>(Bishop)] |= promotions;
    moves[static_cast<Rank::_t>(Knight)] |= promotions;
}

template <Side::_t My>
void PositionMoves::generateKingMoves() {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    //TRICK: our attacks do not hide under attacked king shadow
    Bb kingMoves = ::pieceTypeAttack(King, MY.kingSquare()) % (MY.occ() | attacked);
    moves.set(TheKing, kingMoves);
}

//exclude illegal moves due pin
template <Side::_t My>
void PositionMoves::excludePinnedMoves(VectorPiMask pinnerCandidates) {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    for (Pi pi : pinnerCandidates) {
        Square pinFrom{~OP.squareOf(pi)};
        const Bb& pinLine{::between(MY.kingSquare(), pinFrom)};

        Bb betweenPieces{pinLine & pos.occupied[My]};
        assert (betweenPieces.any());
        if (betweenPieces.isSingleton() && (betweenPieces & MY.occ()).any()) {
            //we discovered a true pinned piece
            Pi pinned{MY.pieceOn(betweenPieces.index())};

            //exclude moves except moves over the pin line
            moves.filter(pinned, pinLine + pinFrom);
        }
    }
}

template <Side::_t My>
void PositionMoves::generateCheckEvasions() {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    VectorPiMask checkers{OP.attacksTo(~MY.kingSquare())};

    if (!checkers.isSingleton()) {
        //multiple (double) checker's case: no moves except king's ones are possible
        moves.clear();
        generateKingMoves<My>();
        return;
    }

    //simple check case
    {
        Pi checker{checkers.index()};
        Square checkFrom{~OP.squareOf(checker)};

        const Bb& checkLine{::between(MY.kingSquare(), checkFrom)};

        //general case: check evasion moves of all pieces
        moves = MY.allAttacks() & (checkLine + checkFrom);

        //we need to correct moves of some pawns
        {
            Bb possiblePawns{checkLine << 8}; //simple pawn push over check line

            //the general case generates invalid diagonal moves to empty squares
            possiblePawns |= (checkLine << 9) % Bb{FileA};
            possiblePawns |= (checkLine << 7) % Bb{FileH};

            for (Square from : MY.occPawns() & possiblePawns) {
                Pi pi{MY.pieceOn(from)};

                Bb b{Bb{from.rankUp()} % pos.occupied[My] & checkLine};
                b += ::pieceTypeAttack(Pawn, from) & checkFrom;

                Rank rank{from.rankUp()};
                moves.set(pi, rank, b[rank]);
            }
        }

        //pawns double push over check line
        {
            Bb possiblePawns{Bb{Rank2} % (Bb{pos.occupied[My]}<<8) & checkLine<<16};
            for (Square from : MY.occPawns() & possiblePawns) {
                Pi pi{MY.pieceOn(from)};
                moves.add(pi, File{from}, Rank4);
            }
        }

        excludePinnedMoves<My>(OP.pinnerCandidates() % checkers);
        generateUnderpromotions<My>();

        if ((OP.enPassants() & checkers).any()) {
            generateEnPassantMoves<My>();
        }

        generateKingMoves<My>();
    }
}

//generate all legal moves from the current position for the current side to move
template <Side::_t My>
void PositionMoves::generateMoves() {
    const Side::_t Op{static_cast<Side::_t>(My ^ Side::Mask)};

    //squares attacked by opponent pieces
    attacked = ~OP.allAttacks().gather();

    if ( attacked[MY.kingSquare()] ) {
        generateCheckEvasions<My>();
        return;
    }

    //the most general case: captures and non captures for all pieces, except pawns
    moves = MY.allAttacks() % MY.occ();

    //rebuild pawn moves
    for (Pi pi : MY.pawns()) {
        Square from{ MY.squareOf(pi) };
        BitRank self{File{from}};
        Rank rank{ up(Rank{from}) };

        BitRank b = moves[rank][pi];
        b &= pos.occupied[My][rank]; //remove "captures" of free squares
        b += self % pos.occupied[My][rank]; //add pawn push
        moves.set(pi, rank, b);

        if (rank == Rank3) {
            //pawns double push
            BitRank r4 = self % pos.occupied[My][rank] % pos.occupied[My][Rank4];
            moves.set(pi, Rank4, r4);
        }
    }

    //generate castling moves
    for (Pi pi : MY.castlings()) {
        if ( ::castlingRules.isLegal(MY.castlingSideOf(pi), pos.occupied[My], attacked) ) {
            moves.add(pi, MY.kingSquare());
        }
    }

    //TRICK: castling encoded as a rook move
    //so we cover the case of pinned castling in Chess960
    excludePinnedMoves<My>(OP.pinnerCandidates());

    //underpromotions for each already tested legal queen promotion
    generateUnderpromotions<My>();

    if (MY.enPassants().any()) {
        generateEnPassantMoves<My>();
    }

    //king moves generated separately
    generateKingMoves<My>();
}

void PositionMoves::limitMoves(std::istream& in, MatrixPiBb& searchmoves, Color color) const {
    while (in) {
        Move m { readMove(in, pos, color) };
        if (m.isNull()) { break; }

        Square from{m.from()};
        if (MY.isOn(from)) {
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

bool PositionMoves::eval(Node& node) {
    for (Pi pi : side(My).alive()) {
        Square from{ side(My).squareOf(pi) };

        for (Square to : moves[pi]) {
            moves.clear(pi, to);

            Position child{pos, from, to};
            if (node(child)) { return true; } //CUT
        }
    }
    return false;
}

index_t PositionMoves::countLegalMoves() const {
   return moves.count();
}

Color PositionMoves::makeMoves(std::istream& in, Color colorToMove) {
    while (in) {
        std::streampos err_pos = in.tellg(); //save the streampos for error reporting

        Move m{ readMove(in, pos, colorToMove) };

        if (m.isNull()) {
            ::fail_if_not(in.eof(), in, err_pos);
            break;
        }

        if (!moves.is(MY.pieceOn(m.from()), m.to())) {
            //detected an illegal move
            ::fail_pos(in, err_pos);
            break;
        }

        colorToMove.flip();
        const_cast<Position&>(pos).swapSides();
        const_cast<Position&>(pos).makeMove<Op>(m.from(), m.to());
        generateMoves<My>();
    }
    return colorToMove;
}

#undef MY
#undef OP
