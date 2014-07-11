#ifndef POSITION_SIDE_HPP
#define POSITION_SIDE_HPP

#include "Evaluation.hpp"
#include "MatrixPiBb.hpp"
#include "VectorPiType.hpp"
#include "VectorPiSquare.hpp"
#include "Zobrist.hpp"

/******************************************************************************
 * static information about pieces of one player's side (either on move or opponent)
 */

//TRICK: all squares are relative to the own side (so king piece is initially on E1 square regardless color)
class PositionSide {
    MatrixPiBb attacks; //squares attacked by a piece and pieces attacking to a square
    VectorPiType types; //type of each alive piece, rooks with castling rights, pawns affected by en passant
    VectorPiSquare squares; //onboard square locations of the alive pieces or 'NoSquare' special value

    Bb piecesBb; //all pieces of the current side
    Bb pawnsBb; //pawns of the current side
    Zobrist zobrist; //Zobrist hash key of pieces of the current side
    Evaluation evaluation;

public:
    PositionSide ();
    PositionSide (const PositionSide&) = delete;
    PositionSide& operator = (const PositionSide&) = default;

    static void swap(PositionSide&, PositionSide&);

    #ifdef NDEBUG
        void assertValid(Pi) const {}
    #else
        void assertValid(Pi) const;
    #endif

    const Bb& occ() const { return piecesBb; }
    const Bb& occPawns() const { return pawnsBb; }
    const MatrixPiBb& allAttacks() const { return attacks; }
    const Zobrist& z() const { return zobrist; }

    bool operator [] (Square sq) const { return piecesBb[sq]; }
    bool isOn(Square sq) const { return squares.isOn(sq); }

    Square squareOf(Pi pi) const { assertValid(pi); return squares.squareOf(pi); }
    Square kingSquare() const { return squareOf(TheKing); }
    Pi pieceOn(Square sq) const { assert ((*this)[sq]); Pi pi = squares.pieceOn(sq); assertValid(pi); return pi; }
    CastlingSide castlingSideOf(Pi pi) const { assert (isCastling(pi)); return squareOf(pi) < kingSquare()? QueenSide:KingSide; }
    VectorPiMask on(Square sq) const { return squares.on(sq); }
    template <Rank::_t Rank> VectorPiMask of() const { return squares.of<Rank>(); }

    template <PieceType::_t Type> bool is(Pi pi) const { assertValid(pi); return types.is<Type>(pi); }
    template <PieceType::_t Type> VectorPiMask of() const { return types.of<Type>(); }
    PieceType typeOf(Pi pi) const { assertValid(pi); return types.typeOf(pi); }

    VectorPiMask alive() const { assert (squares.alive() == types.alive()); return squares.alive(); }
    VectorPiMask pawns() const { return types.of<Pawn>(); }
    VectorPiMask sliders() const { return types.sliders(); }
    bool isSlider(Pi pi) const { assertValid(pi); return types.isSlider(pi); }

    bool hasEnPassant() const { return enPassants().any(); }
    Pi getEnPassant() const { return types.getEnPassant(); }
    Square enPassantSquare() const { return squareOf(getEnPassant()); }
    VectorPiMask enPassants() const { return types.enPassants(); }

    VectorPiMask castlings() const { return types.castlings(); }
    bool isCastling(Pi pi) const { assertValid(pi); return types.isCastling(pi); }

    VectorPiMask pinnerCandidates() const { return types.pinnerCandidates(); }

    VectorPiMask attacksTo(Square a) const { return attacks[a]; }
    VectorPiMask attacksTo(Square a, Square b) const { return attacks[a] | attacks[b]; }
    VectorPiMask attacksTo(Square a, Square b, Square c) const { return attacks[a] | attacks[b] | attacks[c]; }

    void drop(PieceType, Square);
    void drop(Pi, PieceType, Square);
    void capture(Square);
    void clear(PieceType, Square);
    void clear(Pi, PieceType, Square);
    void move(Pi, PieceType, Square, Square);
    void promote(Pi, PieceType, Square, Square);
    void castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo);

    void setLeaperAttack(Pi, PieceType, Square);
    void updateSliderAttacks(VectorPiMask, Bb);

    void setEnPassant(Pi, Square);
    void clearEnPassant(Pi);
    void clearEnPassant() { types.clearEnPassant(); }

    void setCastling(Pi);
    void clearCastling();
    void clearCastling(Pi);

    void updatePinRays(Square);
    void updatePinRays(Square, Pi);

    //used only during initial position setup
    bool setCastling(File);
    bool setCastling(CastlingSide);

};

#endif
