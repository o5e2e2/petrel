#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <cstdint>
#include "Index.hpp"

typedef index_t depth_t; //search tree depth (relative to root)
typedef std::int64_t node_count_t;

enum file_t { FileA, FileB, FileC, FileD, FileE, FileF, FileG, FileH };
typedef Index<8, file_t> File;

enum rank_t { Rank8, Rank7, Rank6, Rank5, Rank4, Rank3, Rank2, Rank1 };
typedef Index<8, rank_t> Rank;

constexpr Rank::_t up(Rank rank) { return static_cast<Rank::_t>(rank-1); }

enum color_t { White, Black };
typedef Index<2, color_t> Color;

enum side_to_move_t { My, Op };
typedef Index<2, side_to_move_t> Side;
constexpr Side::_t operator ~ (Side::_t my) { return static_cast<Side::_t>(my ^ Side::Mask); }

enum castling_side_t { KingSide, QueenSide };
typedef Index<2, castling_side_t> CastlingSide;

enum piece_index_t { TheKing };
typedef Index<16, piece_index_t> Pi; //piece index

enum piece_type_t {
    Queen, Rook, Bishop, Knight, Pawn, King, SpecialPiece, PinRay
};
typedef Index<3, piece_type_t> SliderType;
typedef Index<4, piece_type_t> PromoType;
typedef Index<6, piece_type_t> PieceType;
typedef Index<7, piece_type_t> PieceTag;

enum chess_variant_t { Orthodox, Chess960 };
typedef Index<2, chess_variant_t> ChessVariant;

#endif
