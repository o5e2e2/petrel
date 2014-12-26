#ifndef SEARCH_OUTPUT_HPP
#define SEARCH_OUTPUT_HPP

class SearchInfo;

class SearchOutput {
public:
    virtual void report_current(const SearchInfo&) =0;
    virtual void report_bestmove(const SearchInfo&) =0;
    virtual void report_perft_depth(const SearchInfo&) =0;
    virtual void report_perft_divide(const SearchInfo&) =0;

    virtual ~SearchOutput() {}
};

#endif
