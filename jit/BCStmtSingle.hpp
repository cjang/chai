// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BC_STMT_SINGLE_HPP_
#define _CHAI_BC_STMT_SINGLE_HPP_

#include <cstddef>
#include <set>
#include <stdint.h>
#include <vector>

#include "AstVariable.hpp"
#include "BackMem.hpp"
#include "BCStmt.hpp"
#include "chai/BC.hpp"
#include "chai/Stak.hpp"
#include "FrontMem.hpp"
#include "VectorNut.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// basic bytecode statement

class BCStmtSingle : public BCStmt
{
    const uint32_t _lhsVariable;
    const uint32_t _lhsVersion;
    const Stak<BC> _rhsBytecode;

    const bool     _lhsVariableIsLive;

    // will be empty if no memory allocated for this statement
    std::vector< FrontMem* > _frontMem;
    BackMem*                 _backMem;

    // only has nuts for variables in the statement
    std::map< uint32_t, VectorNut* > _vectorNuts;

    // after other member variables so they exist during initialization
    const uint64_t _hashCode;

    uint64_t computeHash(VectorTrace&);

public:
    BCStmtSingle(const size_t stmtIndex,
                 VectorTrace& vt);

    BCStmtSingle(const size_t stmtIndex,
                 VectorTrace& vt,
                 const Stak<BC>& rhsBytecodeOverride);

    BCStmtSingle(const size_t stmtIndex,
                 VectorTrace& vt,
                 const uint32_t lhsVariableOverride);

    BCStmtSingle(const size_t stmtIndex,
                 VectorTrace& vt,
                 const uint32_t lhsVariableOverride,
                 const Stak<BC>& rhsBytecodeOverride);

    uint32_t lhsVariable(void) const;
    uint32_t lhsVersion(void) const;
    const Stak<BC>& rhsBytecode(void) const;

    bool lhsVariableIsLive(void) const;

    std::vector< FrontMem* >& frontMem(void);
    const std::vector< FrontMem* >& frontMem(void) const;

    BackMem* backMem(void) const;

    uint64_t hashCode(void);

    AstVariable* getAst(void);
    void setAst(AstVariable*);

    void debug(std::ostream&);

    void accept(VisitBCStmt&);
};

}; // chai_internal

#endif
