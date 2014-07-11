/**
* Startup constant initialization
*/
#include <cstdlib>

#include "BetweenSquares.hpp"
#include "CastlingRules.hpp"
#include "Evaluation.hpp"
#include "PieceTypeAttack.hpp"
#include "SearchControl.hpp"
#include "Timer.hpp"
#include "TranspositionTable.hpp"
#include "Uci.hpp"
#include "VectorOf.hpp"
#include "VectorPiOrder.hpp"
#include "VectorPiSingle.hpp"
#include "Zobrist.hpp"

const BetweenSquares between; //32k
const ReverseBb::Direction ReverseBb::direction; //12k
const ReverseBb::Singleton ReverseBb::singleton; //1k
const PieceTypeAttack pieceTypeAttack; //3k
const VectorOf vectorOfAll; //4k
const ZobristTable Zobrist::z; //3k
const VectorPiSingle vectorPiSingle; //256
const ReverseBb::BitReverse ReverseBb::bit_reverse; //64
const VectorBitCount VectorPiRank::popcount; //48
const VectorPiOrder::_t VectorPiOrder::sorted = _mm_setr_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);

const PieceSquareTable Evaluation::pst;

const char* The_piece_type_string{"qrbnpk"};
template <> const char* PieceType::The_string{The_piece_type_string};
template <> const char* PromoType::The_string{The_piece_type_string};
template <> const char* Color::The_string{"wb"};
template <> const char* CastlingSide::The_string{"kq"};
template <> const char* File::The_string{"abcdefgh"};
template <> const char* Rank::The_string{"87654321"};
template <> const char* Pi::The_string{"0123456789abcdef"};

CastlingRules castlingRules; //constant during the same Chess960 game (32)
SearchControl The_game;

Timer::TimerPool Timer::timerPool;

int main(int argc, const char* argv[]) {
    std::ios_base::sync_with_stdio(false); //speed trick
    std::cin.tie(nullptr);

    Uci uci{The_game, std::cout};
    uci(argc, argv); //initialization

    return uci(std::cin, std::clog) ? EXIT_SUCCESS : EXIT_FAILURE;
}
