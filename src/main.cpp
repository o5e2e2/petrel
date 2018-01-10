/**
* Startup constant initialization
*/
#include "io.hpp"
#include "BetweenSquares.hpp"
#include "BitArray128Reverse.hpp"
#include "CastlingRules.hpp"
#include "Evaluation.hpp"
#include "OutsideSquares.hpp"
#include "PerftRecord.hpp"
#include "PieceTypeAttack.hpp"
#include "Timer.hpp"
#include "Uci.hpp"
#include "VectorOf.hpp"
#include "VectorPiOrder.hpp"
#include "VectorPiSingle.hpp"
#include "Zobrist.hpp"

const BetweenSquares between; //32k
const OutsideSquares outside; //32k
const ReverseBb::Direction ReverseBb::direction; //12k
const ReverseBb::Singleton ReverseBb::singleton; //1k
const PieceTypeAttack pieceTypeAttack; //3k
const VectorOf vectorOfAll; //4k
const VectorPiSingle vectorPiSingle; //256
const CastlingRules castlingRules; //128
const ZobristKey Zobrist::zobristKey; //64
const BitReverse bitReverse; //64
const VectorBitCount bitCount; //48
const VectorPiOrder::Sorted VectorPiOrder::sorted; //16

const PieceSquareTable Evaluation::pieceSquareTable; //7*64

using io::literal_type;
template <> literal_type PieceType::The_string{"qrbnpk"};
template <> literal_type PromoType::The_string{"qrbn"};
template <> literal_type Color::The_string{"wb"};
template <> literal_type CastlingSide::The_string{"kq"};
template <> literal_type File::The_string{"abcdefgh"};
template <> literal_type Rank::The_string{"87654321"};
template <> literal_type Pi::The_string{"K123456789abcdef"};

int main(int argc, const char* argv[]) {
    //speed tricks
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if (argc > 1) {
        std::string option{argv[1]};

        if (option == "--version" || option == "-v") {
            std::cout << io::app_copyright;
            return 0;
        }

        if (option == "--help" || option == "-h") {
            std::cout << io::app_usage;
            return 0;
        }
    }

    Uci uci(std::cout, std::cerr);
    uci(std::cin);
}
