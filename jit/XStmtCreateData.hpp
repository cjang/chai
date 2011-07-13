// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_CREATE_DATA_HPP_
#define _CHAI_X_STMT_CREATE_DATA_HPP_

#include "AstVariable.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// create array memory on compute device

class XStmtCreateData : public XStmt
{
    bool   _initializeData;
    float  _floatValue;
    double _doubleValue;

public:
    XStmtCreateData(AstVariable* lhs);

    bool initializeData(void) const;
    float floatValue(void) const;
    double doubleValue(void) const;

    void initializeData(const float value);
    void initializeData(const double value);

    bool swappable(const XStmt&) const;

    void accept(VisitXStmt&);
};

}; // namespace chai_internal

#endif
