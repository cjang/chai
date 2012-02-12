// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BC_HPP_
#define _CHAI_BC_HPP_

#include <stdint.h>

#include "chai/Stak.hpp"

namespace chai_internal {

////////////////////////////////////////
// bytecode stack object

class BC
{
    uint32_t _code;

    enum MASK { SCALAR = 0x4000, PTR = 0x8000, CLEAR = 0x3fff };

    union {
        double _scalar;
        void*  _ptr;
    };

public:
    BC(const uint32_t variable, const uint32_t version);
    BC(const uint32_t opCode);
    BC(const double scalar);
    BC(void* ptr);

    BC(const BC&);
    BC& operator= (const BC&);

    void accept(Visit<BC>&) const;
};

template <>
struct Visit<BC>
{
    virtual void visit(const uint32_t variable, const uint32_t version) = 0;
    virtual void visit(const uint32_t opCode) = 0;
    virtual void visit(const double scalar) = 0;
    virtual void visit(void* ptr) = 0;
};

}; // namespace chai_internal

#endif
