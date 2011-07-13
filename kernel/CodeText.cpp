// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// declarations

void declare(ostream& os, const Variable& v)
{
    v.declareType(os);
    v.identifierName(os);
}

}; // namespace chai_internal
