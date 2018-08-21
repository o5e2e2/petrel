#ifndef UCI_SEARCH_CONTROL_HPP
#define UCI_SEARCH_CONTROL_HPP

#include "io.hpp"
#include "SearchControl.hpp"

class PositionFen;
class UciSearchInfo;

/**
 * Shared data to all search threads (currently the only one)
 */
class UciSearchControl : public SearchControl {
public:
    UciSearchControl (UciSearchInfo&);

    void setHash(io::istream&);
    void go(io::istream&, const PositionFen&);

};

#endif
