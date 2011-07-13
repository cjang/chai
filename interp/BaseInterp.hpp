// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BASE_INTERP_HPP_
#define _CHAI_BASE_INTERP_HPP_

#include <stack>
#include <vector>

#include "BackMem.hpp"
#include "BC.hpp"
#include "FrontMem.hpp"
#include "MemManager.hpp"
#include "Stak.hpp"
#include "FrontMem.hpp"
#include "VectorTrace.hpp"

namespace chai_internal {

////////////////////////////////////////
// common base for all bytecode
// interpreter operations

class BaseInterp : public Visit<BC>
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
    std::vector< std::vector< FrontMem* > > _argStack;

    // context
    VectorTrace* _vt;
    MemManager*  _memManager;

    // implemented in derived class
    virtual void sub_eval(std::stack< std::vector< FrontMem* > >&) = 0;

    // all streams in a vector share the same precision and dimensions
    bool isFloat(const size_t argStackIndex) const;
    bool isDouble(const size_t argStackIndex) const;
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

    // access vector arguments with modulo index arithmetic
    float* floatPtr(const size_t argStackIndex,
                    const size_t vecIndex) const;
    double* doublePtr(const size_t argStackIndex,
                      const size_t vecIndex) const;

    BackMem* allocBackMem(const size_t W,
                          const size_t H,
                          const bool isDP) const;

    FrontMem* allocFrontMem(const size_t W,
                            const size_t H,
                            const bool isDP,
                            BackMem* backMem,
                            const size_t vecIndex) const;

    BaseInterp(const size_t inCount,
           const size_t outCount);

public:
    virtual ~BaseInterp(void);

    void setContext(VectorTrace& vt);
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
