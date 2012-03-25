// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BYTE_TRACE_HPP_
#define _CHAI_BYTE_TRACE_HPP_

#include <cstddef>
#include <map>
#include <stdint.h>
#include <stdlib.h>
#include <vector>

#include "BCStmt.hpp"
#include "VectorTrace.hpp"
#include "VisitBCStmt.hpp"

namespace chai_internal {

////////////////////////////////////////
// vectorized bytecode statement trace

class ByteTrace
{
    VectorTrace& _vt;

    std::vector< BCStmt* > _statements;

    // returns number of representative variables
    size_t liftConstants(void);  // do first

    // returns number of transform passes
    size_t rollLoops(void);      // do second

public:
    ByteTrace(VectorTrace&);
    ~ByteTrace(void);

    VectorTrace& getVectorTrace(void);

    uint64_t hashCode(void) const;
    size_t numTraces(void) const;

    void traverse(VisitBCStmt&);

    void optimizeBytecode(void);
};

}; // namespace chai_internal

#endif
