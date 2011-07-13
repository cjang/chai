// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Qualifier.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// qualifier for variables and functions

Qualifier::Qualifier(const std::string& s)
    : _str(s) { }

Qualifier::~Qualifier(void) { }

string Qualifier::str(void) const
{
    return _str;
}

////////////////////////////////////////
// address space qualifier

AddressSpace::AddressSpace(const std::string& s)
    : Qualifier(s) { }

////////////////////////////////////////
// image access qualifier

ImageAccess::ImageAccess(const std::string& s)
    : Qualifier(s) { }

////////////////////////////////////////
// function attribute qualifier

FunctionAttribute::FunctionAttribute(const std::string& s)
    : Qualifier(s) { }

}; // namespace chai_internal
