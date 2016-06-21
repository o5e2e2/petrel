#ifndef SEARCH_OUTPUT_HPP
#define SEARCH_OUTPUT_HPP

class SearchInfo;

class SearchOutput {
public:
    virtual void bestmove(const SearchInfo&) const {};
    virtual void readyok(const SearchInfo&) const {};

    virtual void info_depth(const SearchInfo&) const {};
    virtual void info_currmove(const SearchInfo&) const {};

    virtual void error(std::istream&) const {};
    virtual void error(const std::string&) const {};

    virtual ~SearchOutput() {}
};

#endif
