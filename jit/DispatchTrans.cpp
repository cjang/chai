// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "DispatchTrans.hpp"

using namespace std;

namespace chai_internal {

DispatchTrans::DispatchTrans(void)
    : _outStack(NULL) { }

DispatchTrans::~DispatchTrans(void)
{
    for (map< uint32_t, BaseTrans* >::const_iterator
         it = _dtable.begin();
         it != _dtable.end();
         it++)
    {
        delete (*it).second;
    }
}

void DispatchTrans::addOp(const uint32_t opCode, BaseTrans* op)
{
    _dtable[opCode] = op;
}

void DispatchTrans::setContext(stack< BaseAst* >& outStack)
{
    _outStack = &outStack;
}

void DispatchTrans::setContext(VectorTrace& vt)
{
    for (map< uint32_t, BaseTrans* >::const_iterator
         it = _dtable.begin();
         it != _dtable.end();
         it++)
    {
        (*it).second->setContext(vt);
    }
}

void DispatchTrans::setContext(MemManager& mm)
{
    for (map< uint32_t, BaseTrans* >::const_iterator
         it = _dtable.begin();
         it != _dtable.end();
         it++)
    {
        (*it).second->setContext(mm);
    }
}

void DispatchTrans::visit(const uint32_t variable, const uint32_t version)
{
    _tmpVariable.push_back(variable);
    _tmpVersion.push_back(version);
}

void DispatchTrans::visit(const uint32_t opCode)
{
    // lookup operation handler
    BaseTrans* op = _dtable[opCode];

    // push any stream arguments first
    for (size_t i = 0; i < _tmpVariable.size(); i++)
    {
        op->push(_tmpVariable[i], _tmpVersion[i], *_outStack);
    }
    _tmpVariable.clear();
    _tmpVersion.clear();

    // dispatch to operation handler
    op->eval(_tmpStak, *_outStack);
}

void DispatchTrans::visit(const double scalar)
{
    _tmpStak.push(scalar);
}

void DispatchTrans::visit(void* ptr)
{
    _tmpStak.push(ptr);
}

}; // namespace chai_internal
