#ifndef POSITION_SIDE_HPP
#define POSITION_SIDE_HPP

#include "Evaluation.hpp"
#include "MatrixPiBb.hpp"
#include "Move.hpp"
#include "PiTrait.hpp"
#include "PiType.hpp"
#include "PiSquare.hpp"
#include "Zobrist.hpp"

//TRICK: all squares are relative to its own side (so the king piece is initially on E1 square regardless color)

/// static information about pieces from one player's side (either side to move or its opponent)
class PositionSide {
    MatrixPiBb attacks; //squares attacked by a piece and pieces attacking to a square
    PiType types; //chess type of each alive piece: king, pawn, knignt, bishop, rook, queen
    PiTrait traits; //rooks with castling rights, pawns affected by en passant, pinner pieces, checker pieces
    PiSquare squares; //onboard square locations of the alive pieces or 'NoSquare' special value

    Evaluation evaluation; //PST incremental evaluation

    Bb piecesBb; //squares occupied by current side pieces
    Bb pawnsBb; //squares of current side pawns
    Bb occupiedBb; //all occupied squares by both sides, updated by combining both PositionSide::piecesBb

    Square opKing; //location of the opponent's king

public:
    constexpr PositionSide () = default;
    PositionSide (const PositionSide&) = default;

    static Score evaluate(const PositionSide&, const PositionSide&);

    Zobrist generateZobrist() const; //calculate Zobrist key from scratch

    #ifdef NDEBUG
        void assertValid(Pi) const {}
        void assertValid(Pi, PieceType, Square) const {}
    #else
        void assertValid(Pi) const;
        void assertValid(Pi, PieceType, Square) const;
    #endif

    const Bb& sideSquares() const { return piecesBb; }
    const Bb& occupied() const { return occupiedBb; }
    bool isOccupied(Square sq) const { return piecesBb.has(sq); }
    PiMask alivePieces() const { assert (squares.alivePieces() == types.alivePieces()); return squares.alivePieces(); }

    Square squareOf(Pi pi) const { assertValid(pi); return squares.squareOf(pi); }
    Square kingSquare() const { return squareOf(TheKing); }

    bool hasPieceOn(Square sq) const { assert (isOccupied(sq) == squares.hasPieceOn(sq)); return squares.hasPieceOn(sq); }
    Pi pieceOn(Square sq) const { Pi pi = squares.pieceOn(sq); assertValid(pi); return pi; }
    PiMask piecesOn(Rank rank) const { return squares.piecesOn(rank); }

    PiMask piecesOfType(PieceType ty) const { return types.piecesOfType(ty); }
    PieceType typeOf(Pi pi) const { assertValid(pi); return types.typeOf(pi); }
    PieceType typeOf(Square sq) const { return typeOf(pieceOn(sq)); }

    const Bb& pawnsSquares() const { return pawnsBb; }
    PiMask pawns() const { return types.piecesOfType(Pawn); }
    bool isPawn(Pi pi) const { assertValid(pi); return types.isPawn(pi); }

    PiMask castlingRooks() const { return traits.castlingRooks(); }
    bool isCastling(Pi pi) const { assertValid(pi); return traits.isCastling(pi); }
    bool isCastling(Square sq) const { return isCastling(pieceOn(sq)); }

    PiMask enPassantPawns() const { return traits.enPassantPawns(); }
    bool hasEnPassant() const { return enPassantPawns().any(); }
    Square enPassantSquare() const { Square ep = squareOf(traits.getEnPassant()); assert (ep.on(Rank4)); return ep; }
    File enPassantFile() const { return File( enPassantSquare() ); }

    PiMask pinners() const { return traits.pinners(); }
    bool isPinned(Bb) const;

    const MatrixPiBb& attacksMatrix() const { return attacks; }
    PiMask attackersTo(Square a) const { return attacks[a]; }
    PiMask attackersTo(Square a, Square b) const { return attacks[a] | attacks[b]; }
    PiMask attackersTo(Square a, Square b, Square c) const { return attacks[a] | attacks[b] | attacks[c]; }

    PiMask checkers() const { assert (traits.checkers() == attacks[opKing]); return traits.checkers(); }

    Move createMove(Square from, Square to) const;

//friend class Position;
    static void swap(PositionSide&, PositionSide&);
    static void finalSetup(PositionSide&, PositionSide&);
    static void syncOccupied(PositionSide&, PositionSide&);

    void setOpKing(Square);
    void move(Pi, Square, Square);
    void movePawn(Pi, Square, Square);
    void moveKing(Square, Square);
    void castle(Square kingFrom, Square kingTo, Pi rook, Square rookFrom, Square rookTo);
    void promote(Pi, PromoType, Square, Square);
    void capture(Square);

    void setEnPassantVictim(Pi);
    void setEnPassantKiller(Pi);
    void clearEnPassantVictim();
    void clearEnPassantKillers();
    void clearCheckers() { traits.clearCheckers(); }

    //TRICK: attacks calculated without opponent's king for implicit out of check king's moves generation
    void setSliderAttacks(PiMask affected) { setSliderAttacks(affected, occupiedBb - opKing); };

    void setGamePhase(const PositionSide&);

    //used only during initial position setup
    bool dropValid(PieceType, Square);

//friend class PositionFen;
    bool setValidCastling(File);
    bool setValidCastling(CastlingSide);

private:
    void move(Pi, PieceType, Square, Square);
    void kingMoved(Square);
    void setSliderAttacks(PiMask, Bb);
    void setLeaperAttacks();
    void setLeaperAttack(Pi, PieceType, Square);
    void setPinner(Pi, PieceType, Square);
    GamePhase generateGamePhase() const; //returns whether material for endgame or middlegame

};

#endif
