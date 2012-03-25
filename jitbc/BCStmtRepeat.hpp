// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BC_STMT_REPEAT_HPP_
#define _CHAI_BC_STMT_REPEAT_HPP_

#include <cstddef>
#include <ostream>
#include <stdint.h>

#include "BCStmtCompound.hpp"
#include "BCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// repeat statement

class BCStmtRepeat : public BCStmt
{
    BCStmtCompound* _stmt;

    const size_t    _reps;

public:
    BCStmtRepeat(const size_t reps, BCStmtCompound*);
    ~BCStmtRepeat(void);

    uint64_t hashCode(void);

    void debug(std::ostream&);

    size_t numReps(void) const;

    BCStmtCompound* stuffInside(void) const;

    void accept(VisitBCStmt&);
};

}; // chai_internal

#endif
