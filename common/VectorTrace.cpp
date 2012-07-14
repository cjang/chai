// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "VectorTrace.hpp"

using namespace std;

namespace chai_internal {

VectorTrace::VectorTrace(const map< pthread_t, SingleTrace* >& traceSet)
    : _numTraces(traceSet.size())
{
    const SingleTrace* t = (*traceSet.begin()).second;

    _hashCode            = t->hashCode();
    _liveVariables       = t->_liveVariables;
    _lhsVariable         = t->_lhsVariable;
    _lhsVersion          = t->_lhsVersion;
    _rhsBytecode         = t->_rhsBytecode;
    _hashCodeHistory     = t->hashCodeHistory();
    _stickyMovement      = t->stickyMovement();

    // statement index -> number of array memory slots
    map< size_t, size_t > stmtSlotCount;

    // iterate over all single traces
    for (map< pthread_t, SingleTrace* >::const_iterator
         it = traceSet.begin(); it != traceSet.end(); it++)
    {
        SingleTrace* sitr = (*it).second;

        // statement pointers to array memory
        for (map< size_t, FrontMem* >::const_iterator
             jt = sitr->_frontMem.begin(); jt != sitr->_frontMem.end(); jt++)
        {
            const size_t stmtIndex = (*jt).first;

            if (0 == stmtSlotCount.count(stmtIndex))
                stmtSlotCount[ stmtIndex ] = 0;

            if ((*jt).second->slotMem().empty())
            {
                _frontMem[ stmtIndex ].push_back( (*jt).second );
                stmtSlotCount[ stmtIndex ]++;
            }
            else
            {
                for (vector< FrontMem* >::const_iterator
                     kt = (*jt).second->slotMem().begin();
                     kt != (*jt).second->slotMem().end();
                     kt++)
                {
                    _frontMem[ stmtIndex ].push_back( *kt );
                }

                stmtSlotCount[ stmtIndex ] += (*jt).second->slotMem().size();
            }

            if (0 == _frontVar.count(stmtIndex))
            {
                const uint32_t varNum = sitr->_frontVar[ stmtIndex ];
                _frontVar[ stmtIndex ] = varNum;
                _varFront[ varNum ] = stmtIndex;
            }
        }

        // for storing array memory values with variables
        for (map< uint32_t, SingleNut* >::const_iterator
             jt = sitr->_variableNuts.begin();
             jt != sitr->_variableNuts.end();
             jt++)
        {
            if (0 == _vectorNuts.count((*jt).first))
            {
                _vectorNuts[(*jt).first] = new VectorNut;
            }

            _vectorNuts[(*jt).first]->push((*jt).second);
        }

        // constrain vector length choices by JIT
        for (map< uint32_t, int >::const_iterator
             jt = sitr->_forceVecLength.begin();
             jt != sitr->_forceVecLength.end();
             jt++)
        {
            _forceVecLength[ (*jt).first ] = (*jt).second;
        }
    }

    // 1. multiple single traces gathered by scheduler into a vector trace
    // 2. vector array data from a single trace forms a vector trace
    for (map< size_t, size_t >::const_iterator
         it = stmtSlotCount.begin(); it != stmtSlotCount.end(); it++)
    {
        const size_t numSlots = (*it).second;

        if (numSlots > _numTraces)
            _numTraces = numSlots;
    }
}

VectorTrace::~VectorTrace(void)
{
    for (map< uint32_t, VectorNut* >::const_iterator
         it = _vectorNuts.begin(); it != _vectorNuts.end(); it++)
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

const vector< uint64_t >& VectorTrace::hashCodeHistory(void) const
{
    return _hashCodeHistory;
}

bool VectorTrace::stickyMovement(void) const
{
    return _stickyMovement;
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

size_t VectorTrace::varFront(const uint32_t traceNum)
{
    if (0 == _varFront.count(traceNum))
    {
        return -1;
    }
    else
    {
        return _varFront[traceNum];
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

void VectorTrace::memallocFront(const uint32_t variable,
                                const size_t frontSize,
                                const vector< size_t >& frontIndex)
{
    _memallocFrontIndex[ variable ]
        = pair< size_t, vector< size_t > >( frontSize, frontIndex );
}

size_t VectorTrace::memallocFrontSize(const uint32_t variable) const
{
    if (_memallocFrontIndex.count(variable))
    {
        return const_cast< VectorTrace* >(this)
                   ->_memallocFrontIndex[variable].first;
    }
    else
    {
        return 0;
    }
}

vector< size_t > VectorTrace::memallocFrontIndex(const uint32_t variable) const
{
    if (_memallocFrontIndex.count(variable))
    {
        return const_cast< VectorTrace* >(this)
                   ->_memallocFrontIndex[variable].second;
    }
    else
    {
        return vector< size_t >();
    }
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

const map< uint32_t, int >& VectorTrace::forceVecLength(void) const
{
    return _forceVecLength;
}

}; // namespace chai_internal
