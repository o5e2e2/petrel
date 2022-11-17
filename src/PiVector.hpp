#ifndef PI_VECTOR_HPP
#define PI_VECTOR_HPP

#include "bitops128.hpp"
#include "typedefs.hpp"

class PiVector {
public:
    typedef i128_t _t;
    typedef u8_t element_type;

    constexpr explicit PiVector (_t e) : v(e) {}

    constexpr operator _t () const { return v; }
    element_type get(Pi pi) const { return static_cast<element_type>(byte[pi]); }

protected:
    union {
        _t v;
        Pi::arrayOf<element_type> byte;
    };

    void set(Pi pi, element_type b) { byte[pi] = b; }

};

#endif
