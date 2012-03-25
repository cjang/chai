// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BC_STMT_COMPOUND_HPP_
#define _CHAI_BC_STMT_COMPOUND_HPP_

#include <ostream>
#include <stdint.h>
#include <vector>

#include "BCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// compound statement

class BCStmtCompound : public BCStmt
{
    std::vector< BCStmt* > _stmts;

    size_t   _hashStmtCount;
    uint64_t _hashCode;

public:
    BCStmtCompound(void);
    ~BCStmtCompound(void);

    uint64_t hashCode(void);

    void debug(std::ostream&);

    const std::vector< BCStmt* >& stuffInside(void) const;

    void accept(VisitBCStmt&);

    void push(BCStmt*);
};

}; // chai_internal

#endif
