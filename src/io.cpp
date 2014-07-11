#include "io.hpp"

void fail_here(std::istream& in) {
    in.setstate(std::ios::failbit);
}

void fail_char(std::istream& in) {
    in.unget();
    fail_here(in);
}

void fail_pos(std::istream& in, std::streampos rewind) {
    in.seekg(rewind);
    fail_here(in);
}

bool operator == (std::istream& in, const char keyword []) {
    if (keyword == nullptr) { return false; }

    in >> std::ws;

    auto before_token = in.tellg();

    for (char c; *keyword != '\0' && in.get(c) && *keyword == c; ++keyword) {}

    if (*keyword == '\0') {
        constexpr auto _EOF = std::char_traits<std::istream::char_type>::eof();

        auto peek = in.peek();
        if (peek == _EOF || std::isspace(peek)) {
            return true;
        }
    }

    in.clear();
    in.seekg(before_token);
    in.clear();

    return false;
}
