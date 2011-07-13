// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_X_STMT_TRACE_HPP_
#define _CHAI_X_STMT_TRACE_HPP_

#include <cstddef>
#include <stdint.h>
#include <vector>

#include "BCStmtTrace.hpp"
#include "VectorTrace.hpp"
#include "XStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// converted statement trace

class XStmtTrace
{
    VectorTrace& _vt;

    std::vector< XStmt* > _stmts;

public:
    XStmtTrace(BCStmtTrace&);
    ~XStmtTrace(void);

    VectorTrace& getVectorTrace(void);

    uint64_t hashCode(void) const;
    size_t numTraces(void) const;

    void reorder(void);
    void together(void);

    void traverse(VisitXStmt&);
};

}; // namespace chai_internal

#endif
