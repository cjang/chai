// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BASE_TRANS_HPP_
#define _CHAI_BASE_TRANS_HPP_

#include <stack>
#include <vector>

#include "BackMem.hpp"
#include "BaseAst.hpp"
#include "chai/BC.hpp"
#include "chai/Stak.hpp"
#include "chai/Visit.hpp"
#include "FrontMem.hpp"
#include "MemManager.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// common base for all bytecode
// JIT operations

class BaseTrans : public Visit<BC>
{
protected:
    // number of array memory (pointer) and scalar arguments
    const size_t _inCount;

    // number of arguments from stream stack
    const size_t _outCount;

    // bytecode input stack arguments
    std::vector< std::vector< FrontMem* > > _argMem;
    std::vector< BackMem* >                 _backMem;
    std::vector< double >                   _argScalar;

    // output stack arguments
    std::vector< BaseAst* >                 _argStack;

    // context
    VectorTrace* _vt;
    MemManager*  _memManager;

    // implemented in derived class
    virtual BaseAst* sub_eval(void) const = 0;

    BaseTrans(const size_t inCount,
              const size_t outCount);

public:
    virtual ~BaseTrans(void);

    virtual BaseTrans* clone(void) const;

    void setContext(VectorTrace& vt);
    void setContext(MemManager& mm);

    void eval(Stak<BC>& inStak,
              std::stack< BaseAst* >& outStack);

    void push(const uint32_t variable,
              const uint32_t version,
              std::stack< BaseAst* >& outStack);

    void visit(const uint32_t variable, const uint32_t version);
    void visit(const uint32_t opCode);
    void visit(const double scalar);
    void visit(void* ptr);
};

}; // namespace chai_internal

#endif
