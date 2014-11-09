#ifndef OUTPUT_SEARCH_HPP
#define OUTPUT_SEARCH_HPP

#include "typedefs.hpp"
#include "Move.hpp"

class OutputSearch {
public:
    virtual void write_info_current() =0;
    virtual void report_bestmove(Move) =0;
    virtual void report_perft_depth(depth_t, node_count_t) =0;
    virtual void report_perft(Move, index_t currmovenumber, node_count_t) =0;
};

#endif
