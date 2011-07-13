// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_INTERP_DISPATCH_HPP_
#define _CHAI_INTERP_DISPATCH_HPP_

#include <map>
#include <stack>
#include <stdint.h>
#include <vector>

#include "BC.hpp"
#include "FrontMem.hpp"
#include "MemManager.hpp"
#include "BaseInterp.hpp"
#include "Stak.hpp"

namespace chai_internal {

////////////////////////////////////////
// dispatch table base for bytecode to
// array stack evaluation operations

class DispatchInterp : public Visit<BC>
{
    // this would be faster as a vector (may have to change to this later)
    // using a map to avoid strong coupling with operation bytecode values
    // and the error prone code that implies
    std::map< uint32_t, BaseInterp* > _dtable;

    // temporary place for function arguments
    Stak<BC> _tmpStak;

    // temporary place for stream arguments
    std::vector< uint32_t > _tmpVariable;
    std::vector< uint32_t > _tmpVersion;

    // output stack
    std::stack< std::vector< FrontMem* > >* _outStack;

public:
    DispatchInterp(void);
    ~DispatchInterp(void);

    void addOp(const uint32_t opCode, BaseInterp* op);

    void setContext(std::stack< std::vector< FrontMem* > >& outStack);
    void setContext(VectorTrace& vt);
    void setContext(MemManager& mm);

    void visit(const uint32_t variable, const uint32_t version);
    void visit(const uint32_t opCode);
    void visit(const double scalar);
    void visit(void* ptr);
};

}; // namespace chai_internal

#endif
