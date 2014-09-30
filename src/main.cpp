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
#include "version.hpp"
#include "Zobrist.hpp"

const BetweenSquares between; //32k
const ReverseBb::Direction ReverseBb::direction; //12k
const ReverseBb::Singleton ReverseBb::singleton; //1k
const PieceTypeAttack pieceTypeAttack; //3k
const VectorOf vectorOfAll; //4k
const Zobrist::Key Zobrist::key; //3k
const VectorPiSingle vectorPiSingle; //256
const ReverseBb::BitReverse ReverseBb::bit_reverse; //64
const VectorBitCount bit_count; //48
const VectorPiOrder::_t VectorPiOrder::sorted = _mm_setr_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);

const PieceSquareTable Evaluation::pst;

template <> io::literal PieceType::The_string{"qrbnpk"};
template <> io::literal PromoType::The_string{"qrbn"};
template <> io::literal Color::The_string{"wb"};
template <> io::literal CastlingSide::The_string{"kq"};
template <> io::literal File::The_string{"abcdefgh"};
template <> io::literal Rank::The_string{"87654321"};
template <> io::literal Pi::The_string{"K123456789abcdef"};

CastlingRules castlingRules; //constant during the same Chess960 game (32)
SearchControl The_game;

Timer::TimerPool Timer::timerPool;

int main(int argc, const char* argv[]) {
    std::ios_base::sync_with_stdio(false); //speed trick
    std::cin.tie(nullptr);

    Uci uci{The_game, std::cout, std::cerr};

    if (argc == 1) {
        //construct startup configuration filename from program's own name
        std::string filename{argv[0]};
        {
            auto pos = filename.find_last_of('/');
            if (pos != std::string::npos) { filename.erase(0, pos+1); }

            pos = filename.find_first_of('.', 1);
            if (pos != std::string::npos) { filename.erase(pos); }

            filename.append(".rc");
        }

        uci(filename);
    }
    else {
        std::string option{argv[1]};
        if (argc > 2 || option == "--version" || option == "--help" || !uci(option)) {
            std::cout << program_version << '\n'
                << "UCI chess engine.\n"
                << "Copyright (C) 2014 Aleks Peshkov, aleks.peshkov@gmail.com\n"
                << "For terms of use contact author.\n"
            ;
            return EXIT_SUCCESS;
        }
    }

    return uci(std::cin) ? EXIT_SUCCESS : EXIT_FAILURE;
}
