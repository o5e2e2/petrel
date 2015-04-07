/**
* Startup constant initialization
*/
#include "BetweenSquares.hpp"
#include "BitArray128Reverse.hpp"
#include "CastlingRules.hpp"
#include "Evaluation.hpp"
#include "PerftTT.hpp"
#include "PieceTypeAttack.hpp"
#include "Timer.hpp"
#include "Uci.hpp"
#include "VectorOf.hpp"
#include "VectorPiOrder.hpp"
#include "VectorPiSingle.hpp"
#include "ZobristKey.hpp"

const BetweenSquares between; //32k
const ReverseBb::Direction ReverseBb::direction; //12k
const ReverseBb::Singleton ReverseBb::singleton; //1k
const PieceTypeAttack pieceTypeAttack; //3k
const VectorOf vectorOfAll; //4k
const ZobristKey zobristKey; //3k
const VectorPiSingle vectorPiSingle; //256
const BitReverse bit_reverse; //64
const VectorBitCount bit_count; //48
const VectorPiOrder::_t VectorPiOrder::sorted = _mm_setr_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
const CastlingRules castlingRules; //128

const PieceSquareTable Evaluation::pst;

template <> io::literal PieceType::The_string{"qrbnpk"};
template <> io::literal PromoType::The_string{"qrbn"};
template <> io::literal Color::The_string{"wb"};
template <> io::literal CastlingSide::The_string{"kq"};
template <> io::literal File::The_string{"abcdefgh"};
template <> io::literal Rank::The_string{"87654321"};
template <> io::literal Pi::The_string{"K123456789abcdef"};

Timer::TimerPool Timer::timerPool;
PerftTT::age_t PerftTT::age = 0;
PerftTT::used_t PerftTT::used = 0;

int main(int argc, const char* argv[]) {
    //speed tricks
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Uci uci(std::cout);

    if (argc > 1) {
        std::string option{argv[1]};

        if (option == "--version" || option == "--help") {
            std::cout << io::app_version << '\n'
                << "UCI chess engine\n"
                << "Copyright (C) 2015 Aleks Peshkov, aleks.peshkov@gmail.com\n"
                << "For terms of use contact author.\n"
            ;
            return 0;
        }

        //try to run the script from the given filename
        uci.call(option);
    }

    return uci(std::cin);
}
