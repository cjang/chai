// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BASE_INTERP_HPP_
#define _CHAI_BASE_INTERP_HPP_

#include <stack>
#include <vector>

#include "BackMem.hpp"
#include "chai/BC.hpp"
#include "chai/Stak.hpp"
#include "FrontMem.hpp"
#include "MemManager.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// common base for all bytecode
// interpreter operations

class BaseInterp : public Visit<BC>
{
    // set from the interpeter
    size_t _uniqueSwizzleKey;

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
    std::vector< std::vector< FrontMem* > > _argStack;

    // context
    VectorTrace* _vt;
    MemManager*  _memManager;

    // implemented in derived class
    virtual void sub_eval(std::stack< std::vector< FrontMem* > >&) = 0;

    // all streams in a vector share the same precision and dimensions
    size_t precision(const size_t argStackIndex) const;
    size_t W(const size_t argStackIndex) const;
    size_t H(const size_t argStackIndex) const;
    size_t frontSize(const size_t argStackIndex) const;
    size_t numTraces() const;

    // modulo array subscript
    size_t idx(const size_t argStackIndex,
               const size_t x,
               const size_t y) const;

    // checkout all vector elements
    void checkout(const size_t argStackIndex) const;

    void swizzle(const size_t argStackIndex) const;

    // access vector arguments with modulo index arithmetic
    float* floatPtr(const size_t argStackIndex,
                    const size_t vecIndex) const;
    double* doublePtr(const size_t argStackIndex,
                      const size_t vecIndex) const;
    int32_t* intPtr(const size_t argStackIndex,
                    const size_t vecIndex) const;
    uint32_t* uintPtr(const size_t argStackIndex,
                      const size_t vecIndex) const;

    BackMem* allocBackMem(const size_t W,
                          const size_t H,
                          const size_t prec) const;

    FrontMem* allocFrontMem(const size_t W,
                            const size_t H,
                            const size_t prec,
                            BackMem* backMem,
                            const size_t vecIndex) const;

    BaseInterp(const size_t inCount,
               const size_t outCount);

public:
    virtual ~BaseInterp(void);

    virtual BaseInterp* clone(void) const;

    void setContext(VectorTrace& vt, const size_t uniqueSwizzleKey);
    void setContext(MemManager& mm);

    void eval(Stak<BC>& inStak,
              std::stack< std::vector< FrontMem* > >& outStack);

    void push(const uint32_t variable,
              const uint32_t version,
              std::stack< std::vector< FrontMem* > >& outStack);

    void visit(const uint32_t variable, const uint32_t version);
    void visit(const uint32_t opCode);
    void visit(const double scalar);
    void visit(void* ptr);
};

}; // namespace chai_internal

#endif
