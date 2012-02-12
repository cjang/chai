// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtCreateData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// create array memory on compute device

XStmtCreateData::XStmtCreateData(AstVariable* lhs,
                                 const bool lexScope)
    : _outerLexicalScope(lexScope),
      _initializeData(false),
      _floatValue(0),
      _doubleValue(0)
{
    // set LHS variable
    lhsVariable(lhs);

    // array memory creation rises to the top
    buoyancyRise();
}

bool XStmtCreateData::outerLexicalScope(void) const
{
    return _outerLexicalScope;
}

bool XStmtCreateData::initializeData(void) const
{
    return _initializeData;
}

float XStmtCreateData::floatValue(void) const
{
    return _floatValue;
}

double XStmtCreateData::doubleValue(void) const
{
    return _doubleValue;
}

void XStmtCreateData::initializeData(const float value)
{
    _initializeData = true;
    _floatValue = value;
}

void XStmtCreateData::initializeData(const double value)
{
    _initializeData = true;
    _doubleValue = value;
}

bool XStmtCreateData::swappable(const XStmt& other) const
{
    return XStmt::swappable(other);
}

void XStmtCreateData::accept(VisitXStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
