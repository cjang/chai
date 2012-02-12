// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "CodeInterfaces.hpp"
#include "CodeType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// interfaces

// Value
IValue::~IValue(void) { }

// Variable
IVariable::~IVariable(void) { }

//Printable
IPrintable::IPrintable(Indent& indent)
    : _indent(indent) { }

IPrintable::~IPrintable(void) { }

ostream& operator<< (ostream& os, const IPrintable& v)
{
    return v.print(os);
}

}; // namespace chai_internal
