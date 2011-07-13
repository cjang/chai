// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "VectorTrace.hpp"

using namespace std;

namespace chai_internal {

VectorTrace::VectorTrace(const uint64_t hashCode,
                         const set< SingleTrace* >& traceSet)
    : _hashCode(hashCode),
      _numTraces(traceSet.size())
{
    const SingleTrace* t = *traceSet.begin();

    _liveVariables = t->_liveVariables;

    _lhsVariable = t->_lhsVariable;
    _lhsVersion = t->_lhsVersion;

    _rhsBytecode = t->_rhsBytecode;

    // iterate over all single traces
    for (set< SingleTrace* >::const_iterator
         it = traceSet.begin();
         it != traceSet.end();
         it++)
    {
        // statement pointers to array memory
        for (map< size_t, FrontMem* >::const_iterator
             jt = (*it)->_frontMem.begin();
             jt != (*it)->_frontMem.end();
             jt++)
        {
            const size_t stmtIndex = (*jt).first;

            _frontMem[ stmtIndex ].push_back( (*jt).second );

            if (0 == _frontVar.count(stmtIndex))
            {
                _frontVar[ stmtIndex ] = (*it)->_frontVar[ stmtIndex ];
            }
        }

        // for storing array memory values with variables
        for (map< uint32_t, SingleNut* >::const_iterator
             jt = (*it)->_variableNuts.begin();
             jt != (*it)->_variableNuts.end();
             jt++)
        {
            if (0 == _vectorNuts.count((*jt).first))
            {
                _vectorNuts[(*jt).first] = new VectorNut;
            }

            _vectorNuts[(*jt).first]->push((*jt).second);
        }
    }
}

VectorTrace::~VectorTrace(void)
{
    for (map< uint32_t, VectorNut* >::const_iterator
         it = _vectorNuts.begin();
         it != _vectorNuts.end();
         it++)
    {
        delete (*it).second;
    }
}

uint64_t VectorTrace::hashCode(void) const
{
    return _hashCode;
}

size_t VectorTrace::numTraces(void) const
{
    return _numTraces;
}

size_t VectorTrace::traceLength(void) const
{
    return _lhsVariable.size();
}

uint32_t VectorTrace::getVariable(const size_t stmtIndex) const
{
    return _lhsVariable[stmtIndex];
}

uint32_t VectorTrace::getVersion(const size_t stmtIndex) const
{
    return _lhsVersion[stmtIndex];
}

Stak<BC>& VectorTrace::getRHS(const size_t stmtIndex)
{
    return _rhsBytecode[stmtIndex];
}

const Stak<BC>& VectorTrace::getRHS(const size_t stmtIndex) const
{
    return _rhsBytecode[stmtIndex];
}

map< size_t, vector< FrontMem* > >& VectorTrace::frontMem(void)
{
    return _frontMem;
}

const map< size_t, vector< FrontMem* > >& VectorTrace::frontMem(void) const
{
    return _frontMem;
}

uint32_t VectorTrace::frontVar(const size_t stmtIndex)
{
    if (0 == _frontVar.count(stmtIndex))
    {
        return -1;
    }
    else
    {
        return _frontVar[stmtIndex];
    }
}

map< size_t, BackMem* >& VectorTrace::backMem(void)
{
    return _backMem;
}

const map< size_t, BackMem* >& VectorTrace::backMem(void) const
{
    return _backMem;
}

map< uint32_t, DeviceMem* >& VectorTrace::traceMem(void)
{
    return _traceMem;
}

const map< uint32_t, DeviceMem* >& VectorTrace::traceMem(void) const
{
    return _traceMem;
}

map< AstVariable*, DeviceMem* >& VectorTrace::splitMem(void)
{
    return _splitMem;
}

const map< AstVariable*, DeviceMem* >& VectorTrace::splitMem(void) const
{
    return _splitMem;
}

map< uint32_t, VectorNut* >& VectorTrace::vectorNuts(void)
{
    return _vectorNuts;
}

const map< uint32_t, VectorNut* >& VectorTrace::vectorNuts(void) const
{
    return _vectorNuts;
}

const set< uint32_t >& VectorTrace::liveVariables(void) const
{
    return _liveVariables;
}

}; // namespace chai_internal
