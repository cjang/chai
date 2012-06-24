// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "BaseInterp.hpp"
#include "MemalignSTL.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// common base for all bytecode
// interpreter operations

size_t BaseInterp::precision(const size_t argStackIndex) const
{
    return _argStack[argStackIndex].front()->precision();
}

size_t BaseInterp::W(const size_t argStackIndex) const
{
    return _argStack[argStackIndex].front()->W();
}

size_t BaseInterp::H(const size_t argStackIndex) const
{
    return _argStack[argStackIndex].front()->H();
}

size_t BaseInterp::frontSize(const size_t argStackIndex) const
{
    const size_t prec = precision(argStackIndex);

    // standard vector length is double2 and float4
    const size_t standardVectorLength = PrecType::vecLength(prec);

    // special case for scalar reductions
    // relies on implicit convention of array dimensions
    const bool specialCaseReadScalar
        = 1 == H(argStackIndex) && 1 == W(argStackIndex);

    return PrecType::sizeOf(prec) *
           (specialCaseReadScalar ? standardVectorLength
                                  : W(argStackIndex) * H(argStackIndex));
}

size_t BaseInterp::numTraces(void) const
{
    return _vt->numTraces();
}

size_t BaseInterp::idx(const size_t argStackIndex,
                       const size_t x,
                       const size_t y) const
{
    return (x % W(argStackIndex)) + (y % H(argStackIndex)) * W(argStackIndex);
}

void BaseInterp::checkout(const size_t argStackIndex) const
{
    for (vector< FrontMem* >::const_iterator
         it = _argStack[argStackIndex].begin();
         it != _argStack[argStackIndex].end();
         it++)
    {
        (*it)->checkout();
    }
}

void BaseInterp::swizzle(const size_t argStackIndex) const
{
    const vector< FrontMem* >& vref = _argStack[argStackIndex];

    for (vector< FrontMem* >::const_iterator
         it = vref.begin();
         it != vref.end();
         it++)
    {
        (*it)->swizzle(_uniqueSwizzleKey);
    }
}

float* BaseInterp::floatPtr(const size_t argStackIndex,
                            const size_t vecIndex) const
{
    const vector< FrontMem* >& vref = _argStack[argStackIndex];
    return vref[vecIndex % vref.size()]->floatPtr();
}

double* BaseInterp::doublePtr(const size_t argStackIndex,
                              const size_t vecIndex) const
{
    const vector< FrontMem* >& vref = _argStack[argStackIndex];
    return vref[vecIndex % vref.size()]->doublePtr();
}

int32_t* BaseInterp::intPtr(const size_t argStackIndex,
                            const size_t vecIndex) const
{
    const vector< FrontMem* >& vref = _argStack[argStackIndex];
    return vref[vecIndex % vref.size()]->intPtr();
}

uint32_t* BaseInterp::uintPtr(const size_t argStackIndex,
                              const size_t vecIndex) const
{
    const vector< FrontMem* >& vref = _argStack[argStackIndex];
    return vref[vecIndex % vref.size()]->uintPtr();
}

BackMem* BaseInterp::allocBackMem(const size_t W,
                                  const size_t H,
                                  const size_t prec) const
{
    return _memManager->allocBackMem(W, H, prec, numTraces());
}

FrontMem* BaseInterp::allocFrontMem(const size_t W,
                                    const size_t H,
                                    const size_t prec,
                                    BackMem* backMem,
                                    const size_t vecIndex) const
{
    return _memManager->allocFrontMem(W, H, prec, backMem, vecIndex);
}

BaseInterp::BaseInterp(const size_t inCount,
                       const size_t outCount)
    : _uniqueSwizzleKey(0),
      _inCount(inCount),
      _outCount(outCount),
      _vt(NULL),
      _memManager(NULL) { }

BaseInterp::~BaseInterp(void) { }

BaseInterp* BaseInterp::clone(void) const
{
    // language extensions override and return an operation handler object
    return NULL;
}

void BaseInterp::setContext(VectorTrace& vt,
                            const size_t uniqueSwizzleKey)
{
    _vt = &vt;
    _uniqueSwizzleKey = uniqueSwizzleKey;
}

void BaseInterp::setContext(MemManager& mm)
{
    _memManager = &mm;
}

void BaseInterp::eval(Stak<BC>& inStak,
                      stack< vector< FrontMem* > >& outStack)
{
    // read arguments from bytecode stack
    _argMem.clear();
    _backMem.clear();
    _argScalar.clear();
    for (size_t i = 0; i < _inCount; i++)
    {
        inStak.pop(*this);
    }

    // read arguments from output stack
    _argStack.clear();
    for (size_t i = 0; i < _outCount; i++)
    {
        _argStack.push_back(outStack.top());
        outStack.pop();
    }

    // derived class does the real work
    sub_eval(outStack);

    // release arguments
    for (size_t i = 0; i < _outCount; i++)
    {
        for (size_t j = 0; j < _argStack[i].size(); j++)
        {
            delete _argStack[i][j]->release();
        }
    }
}

void BaseInterp::push(const uint32_t variable,
                      const uint32_t version,
                      stack< vector< FrontMem* > >& outStack)
{
    outStack.push(_vt->vectorNuts()[variable]->getNutMem(version));
}

void BaseInterp::visit(const uint32_t variable, const uint32_t version)
{
    // never happens, the dispatcher handles this case
}

void BaseInterp::visit(const uint32_t opCode)
{
    // never happens, the dispatcher handles this case
}

void BaseInterp::visit(const double scalar)
{
    _argScalar.push_back(scalar);
}

void BaseInterp::visit(void* ptr)
{
    const size_t stmtIndex = reinterpret_cast< size_t >(ptr);

    _argMem.push_back( _vt->frontMem()[stmtIndex] );

    _backMem.push_back( _vt->backMem().count(stmtIndex)
                            ? _vt->backMem()[stmtIndex]
                            : NULL );
}

}; // namespace chai_internal
