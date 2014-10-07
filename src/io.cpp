#include "io.hpp"

namespace io {
    void fail_here(std::istream& in) {
        in.setstate(std::ios::failbit);
    }

    void fail_char(std::istream& in) {
        in.unget();
        io::fail_here(in);
    }

    void fail_pos(std::istream& in, std::streampos rewind) {
        in.clear();
        in.seekg(rewind);
        io::fail_here(in);
    }

    void fail_rewind(std::istream& in) {
        io::fail_pos(in, 0);
    }

    /**
     * Tries to read the next token and compare it with the given keyword
     * Return:
     * true  -- next token is equals to keyword, shift stream cursor past the token
     * false -- failed to read or to match next token with the given keyword, stream state is not changed
     **/
    bool next(std::istream& in, io::literal keyword) {
        if (keyword == nullptr) {
            return false;
        }

        auto pos_before = in.tellg();
        auto state_before = in.rdstate();

        {
            using std::isspace;
            using std::tolower;

            in >> std::ws;
            while (isspace(*keyword)) { ++keyword; }

            //compare each keyword char with each token char
            while (*keyword != '\0') {
                if (isspace(*keyword)) {
                    if (!isspace(in.peek())) {
                        goto mismatch;
                    }

                    in >> std::ws;
                    while (isspace(*keyword)) { ++keyword; }
                }

                if ( tolower(*keyword++) != tolower(in.get()) ) {
                    goto mismatch;
                }
            }

            //test that token size is equal to keyword size
            {
                auto peek = in.peek();
                if (in.eof() || isspace(peek)) {
                    return true;
                }
            }
        }

    mismatch:
        in.clear(state_before);
        in.seekg(pos_before);
        return false;
    }

    std::ostream& app_version(std::ostream& out) {
        typedef std::ostream::char_type char_type;

        char_type year[] {__DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10], '\0'};

        char_type month[] {
            (__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't')? '1' :
            (__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v')? '1' :
            (__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c')? '1' : '0',

            (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')? '1' :
            (__DATE__[0] == 'F' && __DATE__[1] == 'e' && __DATE__[2] == 'b')? '2' :
            (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')? '3' :
            (__DATE__[0] == 'A' && __DATE__[1] == 'p' && __DATE__[2] == 'r')? '4' :
            (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')? '5' :
            (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')? '6' :
            (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')? '7' :
            (__DATE__[0] == 'A' && __DATE__[1] == 'u' && __DATE__[2] == 'g')? '8' :
            (__DATE__[0] == 'S' && __DATE__[1] == 'e' && __DATE__[2] == 'p')? '9' :
            (__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't')? '0' :
            (__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v')? '1' :
            (__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c')? '2' : '0',

            '\0'
        };

        char_type day[] {(__DATE__[4] == ' ')? '0':__DATE__[4], __DATE__[5], '\0'};

        return out << "Petrel chess " << year << '-' << month << '-' << day
    #ifndef NDEBUG
            << " DEBUG"
    #endif
        ;
    }

}
