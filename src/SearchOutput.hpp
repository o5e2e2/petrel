#ifndef SEARCH_OUTPUT_HPP
#define SEARCH_OUTPUT_HPP

class SearchOutput {
public:
    virtual void bestmove() const {};
    virtual void readyok() const {};
    virtual void info_depth() const {};
    virtual void info_currmove() const {};
    virtual ~SearchOutput() {}
};

#endif
