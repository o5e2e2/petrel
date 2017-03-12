/**
* Startup constant initialization
*/
#include "BetweenSquares.hpp"
#include "OutsideSquares.hpp"
#include "BitArray128Reverse.hpp"
#include "CastlingRules.hpp"
#include "Evaluation.hpp"
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
const VectorBitCount bit_count; //48
const VectorPiOrder::Sorted VectorPiOrder::sorted; //16

const PieceSquareTable Evaluation::pst; //7*64

template <> io::literal PieceType::The_string{"qrbnpk"};
template <> io::literal PromoType::The_string{"qrbn"};
template <> io::literal Color::The_string{"wb"};
template <> io::literal CastlingSide::The_string{"kq"};
template <> io::literal File::The_string{"abcdefgh"};
template <> io::literal Rank::The_string{"87654321"};
template <> io::literal Pi::The_string{"K123456789abcdef"};

int main(int argc, const char* argv[]) {
    //speed tricks
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Uci uci(std::cout, std::cerr);

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

    uci(std::cin);
}
