// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "StmtCreateData.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// create array memory on compute device

StmtCreateData::StmtCreateData(AstVariable* lhs,
                               const bool lexScope)
    : _outerLexicalScope(lexScope),
      _initializeData(false),
      _uintValue(0),
      _intValue(0),
      _floatValue(0),
      _doubleValue(0)
{
    // set LHS variable
    lhsVariable(lhs);

    // array memory creation rises to the top
    buoyancyRise();
}

bool StmtCreateData::outerLexicalScope(void) const
{
    return _outerLexicalScope;
}

bool StmtCreateData::initializeData(void) const
{
    return _initializeData;
}

uint32_t StmtCreateData::uintValue(void) const
{
    return _uintValue;
}

int32_t StmtCreateData::intValue(void) const
{
    return _intValue;
}

float StmtCreateData::floatValue(void) const
{
    return _floatValue;
}

double StmtCreateData::doubleValue(void) const
{
    return _doubleValue;
}

void StmtCreateData::initializeData(const uint32_t value)
{
    _initializeData = true;
    _uintValue = value;
}

void StmtCreateData::initializeData(const int32_t value)
{
    _initializeData = true;
    _intValue = value;
}

void StmtCreateData::initializeData(const float value)
{
    _initializeData = true;
    _floatValue = value;
}

void StmtCreateData::initializeData(const double value)
{
    _initializeData = true;
    _doubleValue = value;
}

bool StmtCreateData::swappable(const Stmt& other) const
{
    return Stmt::swappable(other);
}

void StmtCreateData::accept(VisitStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
