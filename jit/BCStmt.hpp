// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BC_STMT_HPP_
#define _CHAI_BC_STMT_HPP_

#include <ostream>
#include <stdint.h>

#include "VisitBCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// JIT statement from bytecode

struct BCStmt
{
    virtual ~BCStmt(void);

    virtual uint64_t hashCode(void) = 0;

    virtual void debug(std::ostream&) = 0;

    virtual void accept(VisitBCStmt&) = 0;
};

}; // namespace chai_internal

#endif
