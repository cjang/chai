// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_HPP_
#define _CHAI_X_STMT_HPP_

#include <set>
#include <stdint.h>

#include "AstVariable.hpp"
#include "VisitXStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// working AST statement

class XStmt
{
    // used for swappable determination
    std::set< uint32_t >     _traceLHS;
    std::set< uint32_t >     _traceRHS;
    std::set< AstVariable* > _splitLHS;
    std::set< AstVariable* > _splitRHS;

    // used for kernels
    AstVariable*             _lhsVariable;
    std::set< AstVariable* > _rhsVariable;

    int _buoyancy;

protected:
    XStmt(void);

    void lhsVariable(AstVariable*);
    void rhsVariable(AstVariable*);

    void buoyancyRise(void);
    void buoyancyNeutral(void);
    void buoyancySink(void);
    void setBuoyancy(const XStmt&);

public:
    enum Buoyancy { SINK = +1, NEUTRAL = 0, RISE = -1 };

    virtual ~XStmt(void);

    int getBuoyancy(void) const;

    AstVariable* lhsVariable(void) const;
    const std::set< AstVariable* >& rhsVariable(void) const;

    virtual bool swappable(const XStmt&) const = 0;

    virtual void accept(VisitXStmt&) = 0;
};

}; // namespace chai_internal

#endif
