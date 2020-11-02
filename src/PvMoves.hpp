#ifndef PV_MOVES_HPP
#define PV_MOVES_HPP

#include "typedefs.hpp"
#include "Move.hpp"

template<index_t Depth>
class PvMoves {
    std::array< std::array<Move, Depth>, Depth+1> pv;

public:
    PvMoves () { clear(); }

    void clear() {
        for (index_t ply = 0; ply <= Depth; ++ply) {
            pv[ply][0] = Move{};
        }
    }

    void operator() (ply_t ply, Move move) {
        pv[ply][0] = move;
        Move* target = &pv[ply][1];
        Move* source = &pv[ply + 1][0];
        while ((*target++ = *source++));
    }

    operator const Move* () const { return &pv[0][0]; }

};

#endif
