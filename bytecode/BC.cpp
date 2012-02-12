// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "BC.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// bytecode stack object
// (reverse polish notation)

BC::BC(const uint32_t variable, const uint32_t version)
    : _code((variable + 1) | PTR),    // (variable + 1) != 0 -> _code != PTR
      _ptr(reinterpret_cast<void*>(version)) { }

BC::BC(const uint32_t opCode)
    : _code((opCode + 1) | SCALAR),   // (opCode + 1) != 0 -> _code != SCALAR
      _scalar(0) { }

BC::BC(const double scalar)
    : _code(SCALAR),                  // _code == SCALAR
      _scalar(scalar) { }

BC::BC(void* ptr)
    : _code(PTR),                     // _code == PTR
      _ptr(ptr) { }

BC::BC(const BC& other)
    : _code(other._code)
{
    if (_code & SCALAR)
        _scalar = other._scalar;
    else
        _ptr = other._ptr;
}

BC& BC::operator= (const BC& other)
{
    _code = other._code;

    if (_code & SCALAR)
        _scalar = other._scalar;
    else
        _ptr = other._ptr;

    return *this;
}

void BC::accept(Visit<BC>& v) const
{
    if (_code & SCALAR)
    {
        if (SCALAR == _code)
            v.visit(_scalar);                   // double argument
        else
            v.visit((_code & CLEAR) - 1);       // op code
    }
    else
    {
        if (PTR == _code)
            v.visit(_ptr);                      // pointer argument
        else
            // variable version
            v.visit((_code & CLEAR) - 1, reinterpret_cast<uint64_t>(_ptr));
    }
}

}; // namespace chai_internal
