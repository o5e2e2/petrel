#include "io.hpp"

void fail_here(std::istream& in) {
    in.setstate(std::ios::failbit);
}

void fail_char(std::istream& in) {
    in.unget();
    fail_here(in);
}

void fail_pos(std::istream& in, std::streampos undo) {
    in.seekg(undo);
    fail_here(in);
}

void fail_if_not(bool predicate, std::istream& in, std::streampos undo) {
    if (!predicate) { fail_pos(in, undo); }
}

void fail_if_any(std::istream& in) {
    if ( !(in == "") ) { fail_here(in); }
}

bool operator == (std::istream& in, const char keyword []) {
    constexpr auto _EOF = std::char_traits<std::istream::char_type>::eof();

    in >> std::ws;

    if (keyword == nullptr) {
        return in.peek() == _EOF;
    }

    auto begin = in.tellg();

    for (char c; *keyword != '\0' && in.get(c) && *keyword == c; ++keyword) {}

    if (*keyword == '\0') {
        auto peek = in.peek();
        if (peek == _EOF || std::isspace(peek)) {
            return true;
        }
    }

    in.clear();
    in.seekg(begin);
    in.clear();

    return false;
}
