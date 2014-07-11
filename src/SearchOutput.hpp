#ifndef SEARCH_OUTPUT_HPP
#define SEARCH_OUTPUT_HPP

#include "typedefs.hpp"
#include "Move.hpp"

class SearchOutput {
public:
    virtual void write_info_current() =0;
    virtual void report_bestmove(Move) =0;
    virtual void report_perft_depth(Ply, node_count_t) =0;
    virtual void report_perft(Move, index_t currmovenumber, node_count_t) =0;
};

#endif
