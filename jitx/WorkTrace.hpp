// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_WORK_TRACE_HPP_
#define _CHAI_WORK_TRACE_HPP_

#include <cstddef>
#include <map>
#include <stdint.h>
#include <vector>

#include "AstVariable.hpp"
#include "ByteTrace.hpp"
#include "GroupTogether.hpp"
#include "Stmt.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// converted statement trace

class WorkTrace
{
    VectorTrace& _vt;

    std::vector< Stmt* > _stmts;

    std::map< uint32_t, size_t > _traceReadoutDim;

    GroupTogether _together;

public:
    WorkTrace(ByteTrace&);
    ~WorkTrace(void);

    VectorTrace& getVectorTrace(void);

    uint64_t hashCode(void) const;
    size_t numTraces(void) const;

    void reorder(void);
    void together(void);

    void traverse(VisitStmt&);
};

}; // namespace chai_internal

#endif
