#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <cstdint>
#include <limits>
#include "Index.hpp"

typedef index_t ply_t; //search tree depth (relative to root)
enum : ply_t {
#ifndef NDEBUG
    DepthMax = 20
#else
    DepthMax = 1000
#endif
};

typedef std::uint64_t node_count_t;
enum : node_count_t {
    NodeCountNone = std::numeric_limits<node_count_t>::max(),
    NodeCountMax  = NodeCountNone - 1
};

enum file_t { FileA, FileB, FileC, FileD, FileE, FileF, FileG, FileH };
typedef Index<8, file_t> File;

enum rank_t { Rank8, Rank7, Rank6, Rank5, Rank4, Rank3, Rank2, Rank1 };
typedef Index<8, rank_t> Rank;
constexpr Rank::_t rankForward(Rank rank) { return static_cast<Rank::_t>(rank - Rank7 + Rank8); }

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
    Queen = 0,
    Rook = 1,
    Bishop = 2,
    Knight = 3,
    Pawn = 4,
    King = 5,
    Castling = 6, KingEndgame = Castling,
    Special = 7, EnPassant = Special, Pinner = Special
};
typedef Index<3, piece_type_t> SliderType;
typedef Index<4, piece_type_t> PromoType;
typedef Index<6, piece_type_t> PieceType;
typedef Index<7, piece_type_t> PieceEvalType;

constexpr bool isSlider(PromoType::_t ty) { return ty < Knight; }
constexpr bool isLeaper(PromoType::_t ty) { return ty >= Knight; }

//encoding of the promoted piece type inside 12-bit move
constexpr Rank::_t rankOf(PromoType::_t ty) { return static_cast<Rank::_t>(ty); }

enum chess_variant_t { Orthodox, Chess960 };
typedef Index<2, chess_variant_t> ChessVariant;

enum game_phase_t { Middlegame, Endgame };
typedef Index<2, game_phase_t> GamePhase;

#endif
