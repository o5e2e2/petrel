#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <cstdint>
#include <limits>
#include "Index.hpp"

typedef index_t depth_t; //search tree depth (relative to root)
typedef std::uint64_t node_count_t;
const auto NODE_COUNT_NONE = std::numeric_limits<node_count_t>::max();

enum color_t { White, Black };
typedef Index<2, color_t> Color;

enum side_to_move_t { My, Op };
typedef Index<2, side_to_move_t> Side;
constexpr Side::_t operator ~ (Side::_t my) { return static_cast<Side::_t>(my ^ Side::Mask); }

enum castling_side_t { KingSide, QueenSide };
typedef Index<2, castling_side_t> CastlingSide;

enum piece_index_t { TheKing };
typedef Index<16, piece_index_t> Pi; //piece index

enum piece_type_t { Queen, Rook, Bishop, Knight, Pawn, King, Castling, EnPassant };
typedef Index<3, piece_type_t> SliderType;
typedef Index<4, piece_type_t> PromoType;
typedef Index<6, piece_type_t> PieceType;

enum chess_variant_t { Orthodox, Chess960 };
typedef Index<2, chess_variant_t> ChessVariant;

#endif
