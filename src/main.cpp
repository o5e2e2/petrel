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
const PieceTypeAttack pieceTypeAttack; //3k
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

std::string basename(const std::string& path) {
#if defined(WIN32) || defined(_WIN32)
    const auto The_path_separator = "/\\";
#else
    const auto The_path_separator = '/';
#endif

    auto pos = path.find_last_of(The_path_separator);
    if (pos == std::string::npos ) {
        return path;
    }
    return path.substr(pos + 1);
}

int main(int argc, const char* argv[]) {
    //speed tricks
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cerr.tie(nullptr);

    if (argc == 1) {
        Uci uci(std::cout);
        uci(std::cin, std::cerr);
        return 0;
    }

    std::string command = basename(argv[0]);

    if (argc == 2) {
        std::string option = argv[1];

        if (option == "--help" || option == "-h") {
            std::cout
                << "Usage: " << command << " [OPTION]\n"
                << "UCI Chess engine. Plays the game of chess.\n"
                << "\n"
                << "  -h, --help        display this help and exit\n"
                << "  -v, --version     output version information and exit\n"
            ;
            return 0;
        }

        if (option == "--version" || option == "-v") {
            std::cout << "petrel " << io::app_version << "Written by Aleks Peshkov\n";
            return 0;
        }
    }

    std::cerr << command << ": invalid option\n";
    std::cerr << "Try '" << command <<" --help' for more information.\n";
    return EINVAL;
}
