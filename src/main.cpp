#include <errno.h>

#include "io.hpp"
#include "AttackBb.hpp"
#include "BetweenSquares.hpp"
#include "BitReverse.hpp"
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
constexpr const BetweenSquares between; //32k
constexpr const HyperbolaDir hyperbolaDir; //12k
constexpr const HyperbolaSq hyperbolaSq; //1k
constexpr const PieceTypeAttack attacksFrom; //3k
constexpr const VectorOf vectorOfAll; //4k
constexpr const VectorPiSingle vectorPiSingle; //256
constexpr const PieceSquareTable pieceSquareTable; //1.5k 6*64*4
constexpr const ZobristKey zobristKey; //64
constexpr const CastlingRules castlingRules; //128
constexpr const BitReverse bitReverse; //32
constexpr const VectorBitCount bitCount; //16
const VectorPiOrder::Sorted VectorPiOrder::sorted; //16

using io::czstring;
template <> czstring PieceType::The_string{"qrbnpk"};
template <> czstring PromoType::The_string{"qrbn"};
template <> czstring Color::The_string{"wb"};
template <> czstring CastlingSide::The_string{"kq"};
template <> czstring File::The_string{"abcdefgh"};
template <> czstring Rank::The_string{"87654321"};
template <> czstring Pi::The_string{"K123456789abcdef"};

int main(int argc, const char* argv[]) {
    if (argc > 1) {
        std::string option = argv[1];

        if (option == "--help" || option == "-h") {
            io::option_help(std::cout);
            return EXIT_SUCCESS;
        }

        if (option == "--version" || option == "-v") {
            io::option_version(std::cout);
            return EXIT_SUCCESS;
        }

        io::option_invalid(std::cerr);
        return EINVAL;
    }

    //speed tricks
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cerr.tie(nullptr);

    Uci uci(std::cout);
    uci(std::cin, std::cerr);

    //TRICK: simple "return 0;" does not work
    std::exit(EXIT_SUCCESS);
}
