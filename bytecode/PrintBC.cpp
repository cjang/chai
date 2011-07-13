// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "PrintBC.hpp"

using namespace std;

namespace chai_internal {

PrintBC::PrintBC(ostream& os, const string& sep)
    : _os(os),
      _sep(sep),
      _ptrValue(NULL) { }

size_t PrintBC::ptrIndex(void) const
{
    return _ptrValue ? reinterpret_cast< size_t >(_ptrValue) : -1;
}

void PrintBC::visit(const uint32_t variable, const uint32_t version)
{
    _os << variable << "." << version << _sep;
}

void PrintBC::visit(const uint32_t opCode)
{
    _os << opCode << _sep;
}

void PrintBC::visit(const double scalar)
{
    _os << scalar << _sep;
}

void PrintBC::visit(void* ptr)
{
    _os << "PTR" << _sep;

    _ptrValue = ptr;
}

}; // namespace chai_internal
