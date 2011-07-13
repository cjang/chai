// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_DISPATCH_TRANS_HPP_
#define _CHAI_DISPATCH_TRANS_HPP_

#include <map>
#include <set>
#include <stack>
#include <stdint.h>
#include <vector>

#include "BaseAst.hpp"
#include "BaseTrans.hpp"
#include "BC.hpp"
#include "MemManager.hpp"
#include "Stak.hpp"
#include "Visit.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatch table base for bytecode to
// JIT AST stack evaluation operations

class DispatchTrans : public Visit<BC>
{
    // this would be faster as a vector (may have to change to this later)
    // using a map to avoid strong coupling with operation bytecode values
    // and the error prone code that implies
    std::map< uint32_t, BaseTrans* > _dtable;

    // temporary place for function arguments
    Stak<BC> _tmpStak;

    // temporary place for variable arguments
    std::vector< uint32_t > _tmpVariable;
    std::vector< uint32_t > _tmpVersion;

    // output stack
    std::stack< BaseAst* >* _outStack;

public:
    DispatchTrans(void);
    ~DispatchTrans(void);

    void addOp(const uint32_t opCode, BaseTrans* op);

    void setContext(std::stack< BaseAst* >& outStack);
    void setContext(VectorTrace& vt);
    void setContext(MemManager& mm);

    void visit(const uint32_t variable, const uint32_t version);
    void visit(const uint32_t opCode);
    void visit(const double scalar);
    void visit(void* ptr);
};

}; // namespace chai_internal

#endif
