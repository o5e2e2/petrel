#include <errno.h>

#include "io.hpp"
#include "AttackBb.hpp"
#include "AttacksFrom.hpp"
#include "BitReverse.hpp"
#include "CastlingRules.hpp"
#include "Evaluation.hpp"
#include "PerftRecord.hpp"
#include "PiOrder.hpp"
#include "PiSingle.hpp"
#include "SquaresInBetween.hpp"
#include "Uci.hpp"
#include "VectorOfAll.hpp"
#include "Zobrist.hpp"

/**
* Startup constant initialization
*/
const SquaresInBetween inBetween; //32k 64*64*8
const HyperbolaDir hyperbolaDir; ////12k 3*64*4*16
const HyperbolaSq hyperbolaSq; //1k 64*16
const AttacksFrom attacksFrom; //3k 6*64*8
constexpr const VectorOfAll vectorOfAll; //4k 256*16
const PieceSquareTable pieceSquareTable; //3k 6*64*8
constexpr const PiSingle piSingle; //256
const CastlingRules castlingRules; //128
constexpr const ZobristKey zobristKey; //64 8*8
constexpr const BitReverse bitReverse; //32
constexpr const VectorBitCount bitCount; //16
const PiOrder::Sorted PiOrder::sorted; //16

template <> io::czstring PieceType::The_string{"qrbnpk"};
template <> io::czstring Color::The_string{"wb"};
template <> io::czstring CastlingSide::The_string{"kq"};
template <> io::czstring Pi::The_string{"KQRrBbNn12345678"};

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
