#include <errno.h>

#include "io.hpp"
#include "BetweenSquares.hpp"
#include "BitArray128Reverse.hpp"
#include "CastlingRules.hpp"
#include "Evaluation.hpp"
#include "PerftRecord.hpp"
#include "PieceTypeAttack.hpp"
#include "Uci.hpp"
#include "VectorOf.hpp"
#include "VectorPiOrder.hpp"
#include "VectorPiSingle.hpp"
#include "Zobrist.hpp"

/**
* Startup constant initialization
*/
const BetweenSquares between; //32k
const ReverseBb::Direction ReverseBb::direction; //12k
const ReverseBb::Singleton ReverseBb::singleton; //1k
const PieceTypeAttack attacksFrom; //3k
constexpr const VectorOf vectorOfAll; //4k
const VectorPiSingle vectorPiSingle; //256
constexpr const PieceSquareTable Evaluation::pieceSquareTable; //7*64
constexpr const ZobristKey Zobrist::zobristKey; //64
const CastlingRules castlingRules; //128
const BitReverse bitReverse; //64
const VectorBitCount bitCount; //48
constexpr const VectorPiOrder::Sorted VectorPiOrder::sorted; //16

using io::czstring;
template <> czstring PieceType::The_string{"qrbnpk"};
template <> czstring PromoType::The_string{"qrbn"};
template <> czstring Color::The_string{"wb"};
template <> czstring CastlingSide::The_string{"kq"};
template <> czstring File::The_string{"abcdefgh"};
template <> czstring Rank::The_string{"87654321"};
template <> czstring Pi::The_string{"K123456789abcdef"};

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        //speed tricks
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
        std::cerr.tie(nullptr);

        Uci uci(std::cout);
        uci(std::cin, std::cerr);

        //TRICK: return 0; does not work
        std::exit(0);
    }

    if (argc == 2) {
        std::string option = argv[1];

        if (option == "--help" || option == "-h") {
            std::cout
                << "petrel [-hv]\n"
                << "    Petrel UCI chess engine. Plays the game of chess.\n\n"
                << "    Options:\n"
                << "      -h, --help        display this help and exit\n"
                << "      -v, --version     output version information and exit\n"
            ;
            return 0;
        }

        if (option == "--version" || option == "-v") {
            std::cout
                << "petrel " << io::app_version << '\n'
                << "written by Aleks Peshkov (aleks.peshkov@gmail.com)\n"
            ;
            return 0;
        }
    }

    std::cerr << "petrel: invalid option\n";
    return EINVAL;
}
