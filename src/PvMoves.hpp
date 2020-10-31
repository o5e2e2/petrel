#ifndef PV_MOVES_HPP
#define PV_MOVES_HPP

#include "typedefs.hpp"
#include "Move.hpp"

template<index_t Depth>
class PvMoves {
    std::array<index_t, Depth> index;
    std::array<Move, Depth*(Depth+1)/2> pv;

public:
    constexpr PvMoves () {
        index[0] = 0;
        pv[0] = Move{};
        for (index_t ply = 1; ply < Depth; ++ply) {
            index[ply] = index[ply-1] + Depth - ply - 1;
            pv[index[ply]] = Move{};
        }
    }

    void operator() (ply_t ply, Move move) {
        pv[index[ply]] = move;
        pv[index[ply]+1] = Move{};

        Move* source = &pv[index[ply + 1]];
        Move* target = &pv[index[ply] + 1];

        index_t n = Depth - ply - 1;
        while (n-- && (*target++ = *source++));
    }

    operator const Move* () const { return &pv[0]; }

};

#endif
