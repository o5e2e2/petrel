#ifndef SEARCH_OUTPUT_HPP
#define SEARCH_OUTPUT_HPP

class SearchInfo;

class SearchOutput {
public:
    virtual void readyok(const SearchInfo&) const =0;
    virtual void bestmove(const SearchInfo&) const =0;
    virtual void info_depth(const SearchInfo&) const =0;
    virtual void info_currmove(const SearchInfo&) const =0;

    virtual ~SearchOutput() {}
};

#endif
