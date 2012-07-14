// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_CREATE_DATA_HPP_
#define _CHAI_STMT_CREATE_DATA_HPP_

#include <stdint.h>

#include "AstVariable.hpp"
#include "Stmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// create array memory on compute device

class StmtCreateData : public Stmt
{
    const bool _outerLexicalScope;

    bool     _initializeData;

    uint32_t _uintValue;
    int32_t  _intValue;
    float    _floatValue;
    double   _doubleValue;

public:
    StmtCreateData(AstVariable* lhs, const bool lexScope);

    bool outerLexicalScope(void) const;

    bool initializeData(void) const;

    uint32_t uintValue(void) const;
    int32_t intValue(void) const;
    float floatValue(void) const;
    double doubleValue(void) const;

    void initializeData(const uint32_t value);
    void initializeData(const int32_t value);
    void initializeData(const float value);
    void initializeData(const double value);

    bool randomness(void) const;

    bool swappable(const Stmt&) const;

    void accept(VisitStmt&);
};

}; // namespace chai_internal

#endif
