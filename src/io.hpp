#ifndef IO_HPP
#define IO_HPP

#include <istream>

void fail_here(std::istream&);
void fail_char(std::istream&);
void fail_pos(std::istream&, std::streampos);
void fail_rewind(std::istream&);

bool operator == (std::istream&, const std::istream::char_type keyword []);

#endif
