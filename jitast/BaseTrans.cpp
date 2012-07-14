// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "AstArrayMem.hpp"
#include "AstVariable.hpp"
#include "BackMem.hpp"
#include "BaseTrans.hpp"
#include "FrontMem.hpp"
#include "MemalignSTL.hpp"

using namespace std;

namespace chai_internal {

BaseTrans::BaseTrans(const size_t inCount,
                     const size_t outCount)
    : _inCount(inCount),
      _outCount(outCount),
      _vt(NULL),
      _memMgr(NULL) { }

BaseTrans::~BaseTrans(void) { }

BaseTrans* BaseTrans::clone(void) const
{
    // language extensions override and return an operation handler object
    return NULL;
}

void BaseTrans::setContext(VectorTrace& vt)
{
    _vt = &vt;
}

void BaseTrans::setContext(MemTrans& mm)
{
    _memMgr = &mm;
}

void BaseTrans::eval(Stak<BC>& inStak,
                     stack< BaseAst* >& outStack)
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
    outStack.push(sub_eval());
}

void BaseTrans::push(const uint32_t variable,
                     const uint32_t version,
                     stack< BaseAst* >& outStack)
{
    const vector< FrontMem* > frontMem = _vt->vectorNuts()[variable]
                                            ->getNutMem(version);

    if (frontMem.empty())
    {
        // AST variable object
        outStack.push(
            _vt->vectorNuts()[variable]
               ->getNutVar(version) );
    }
    else
    {
        // intermediate variable value is array memory
        outStack.push(
            new AstArrayMem(
                    frontMem,
                    _memMgr->unifyBackMem(variable, version, frontMem),
                    variable,
                    version ) );
    }
}

void BaseTrans::visit(const uint32_t variable, const uint32_t version)
{
    // never happens, the dispatcher handles this case
}

void BaseTrans::visit(const uint32_t opCode)
{
    // never happens, the dispatcher handles this case
}

void BaseTrans::visit(const double scalar)
{
    _argScalar.push_back(scalar);
}

void BaseTrans::visit(void* ptr)
{
    const size_t stmtIndex = reinterpret_cast< size_t >(ptr);

    _argMem.push_back( _vt->frontMem()[stmtIndex] );

    _backMem.push_back( _vt->backMem().count(stmtIndex)
                            ? _vt->backMem()[stmtIndex]
                            : NULL );
}

}; // namespace chai_internal
