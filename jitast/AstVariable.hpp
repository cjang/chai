// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_AST_VARIABLE_HPP_
#define _CHAI_AST_VARIABLE_HPP_

#include <stdint.h>
#include <vector>

#include "AstArrayMem.hpp"
#include "BaseAst.hpp"
#include "FrontMem.hpp"

namespace chai_internal {

////////////////////////////////////////
// represents a LHS/RHS variable

class AstVariable : public BaseAst
{
    // corresponds to trace LHS
    const bool     _isTraceVariable;
    const bool     _isLiveVariable;
    const uint32_t _variable;
    const uint32_t _version;

    // trace variable has front/back memory assigned to it
    const std::vector< FrontMem* > _frontMem;

    // three general kinds of variables in the JIT middle-end
    enum Kindness
    {
        SPLIT_ARRAY_MEMORY,  // always a read-only argument to a kernel
        SPLIT_OPERATION,     // write-only operation output, read-only input
        TRACE_VARIABLE       // may be: read-only, write-only, read-write
    };

    // the kind of this variable
    const Kindness _kindOfVariable;
    const bool     _sameDataAcrossTraces;

    // do not use dot() hack for scalars when operation split reduction
    bool _disableDotHack;

public:
    // new variable is split off array memory
    AstVariable(AstArrayMem* barg);

    // new introduced variable not from trace
    AstVariable(BaseAst* barg,
                const bool writable);

    // variable from trace
    AstVariable(BaseAst* barg,
                const uint32_t variable,
                const uint32_t version,
                const bool isLive,
                const std::vector< FrontMem* >& frontMem);

    bool isTraceVariable(void) const;
    bool isLiveVariable(void) const;
    uint32_t variable(void) const;
    uint32_t version(void) const;

    const std::vector< FrontMem* >& frontMem(void) const;

    bool isSameDataAcrossTraces(void) const;

    bool isReadOnly(const bool appearsOnLHS, const bool appearsOnRHS) const;
    bool isWriteOnly(const bool appearsOnLHS, const bool appearsOnRHS) const;
    bool isReadWrite(const bool appearsOnLHS, const bool appearsOnRHS) const;

    bool enableDotHack(void) const;
    void disableDotHack(void);

    void accept(VisitAst&);
};

}; // namespace chai_internal

#endif
