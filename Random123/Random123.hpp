// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_RANDOM123_HPP_
#define _CHAI_RANDOM123_HPP_

#include <ostream>
#include <set>

#include "R123Util.hpp"

namespace chai_internal {

////////////////////////////////////////
// Random123 OpenCL for the JIT

class Random123 : public R123Util
{
    std::set< size_t > _precSet;
    std::set< size_t > _vecLenSet;

    bool _philox;
    bool _threefry;

    // integer typedefs
    void typedefDecl(std::ostream& os) const;

    // array
    void arrayDecl(std::ostream& os) const;
    void arrayDecl(std::ostream& os, const size_t PR, const size_t VL) const;

public:
    Random123(void);

    void prec32(void);
    void prec64(void);

    void vecLength2(void);
    void vecLength4(void);

    void philox(void);
    void threefry(void);

    void print(std::ostream&);
};

}; // namespace chai_internal

#endif
