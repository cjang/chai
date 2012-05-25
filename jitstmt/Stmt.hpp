// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_STMT_HPP_
#define _CHAI_STMT_HPP_

#include <set>
#include <stdint.h>

#include "AstVariable.hpp"
#include "VisitStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// working AST statement

class Stmt
{
    // used for swappable determination
    std::set< uint32_t >     _traceLHS;
    std::set< uint32_t >     _traceRHS;
    std::set< AstVariable* > _splitLHS;
    std::set< AstVariable* > _splitRHS;

    // used for kernels, set by AST descent
    AstVariable*              _lhsVariable;
    std::set< AstVariable* >  _rhsVariable;

    // used for kernels, set externally
    std::set< AstVariable* > _underlyingVars;
    bool                     _scalarVectorLength;

    bool _constructorLHS;
    bool _destructorLHS;

    int  _buoyancy;
    bool _surfaceBuoyancy;

protected:
    Stmt(void);

    void lhsVariable(AstVariable*);
    void rhsVariable(AstVariable*);

    void buoyancySurface(void);
    void buoyancyRise(void);
    void buoyancyNeutral(void);
    void buoyancySink(void);
    void setBuoyancy(const Stmt&);

public:
    enum Buoyancy { SINK = +1, NEUTRAL = 0, RISE = -1 };

    virtual ~Stmt(void);

    int getBuoyancy(void) const;
    bool surfaceBuoyancy(void) const;

    bool getConstructorLHS(void) const;
    bool getDestructorLHS(void) const;
    void setConstructorLHS(void);
    void setDestructorLHS(void);

    AstVariable* lhsVariable(void) const;
    const std::set< AstVariable* >& rhsVariable(void) const;

    const std::set< AstVariable* >& underlyingVars(void) const;
    void underlyingVars(const std::set< AstVariable* >& underlyingVars);

    bool scalarVectorLength(void) const;
    void scalarVectorLength(const bool);

    virtual bool trackLHS(void) const;

    virtual bool swappable(const Stmt&) const = 0;

    virtual void accept(VisitStmt&) = 0;
};

}; // namespace chai_internal

#endif
