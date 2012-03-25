// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>

#include "ElideInternal.hpp"
#include "MakeStmt.hpp"
#include "StmtIdSpace.hpp"
#include "TrimFat.hpp"
#include "UseRegister.hpp"
#include "WorkTrace.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// converted statement trace

WorkTrace::WorkTrace(ByteTrace& trace)
    : _vt(trace.getVectorTrace())
{
    MakeStmt v(_stmts, _vt);

    trace.traverse(v);

    _traceReadoutDim = v.traceReadoutDim();

    if (v.reductionInsideLoop())
        _together.reductionIsSpecial(false);
}

WorkTrace::~WorkTrace(void)
{
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        delete *it;
    }
}

VectorTrace& WorkTrace::getVectorTrace(void)
{
    return _vt;
}

uint64_t WorkTrace::hashCode(void) const
{
    return _vt.hashCode();
}

size_t WorkTrace::numTraces(void) const
{
    return _vt.numTraces();
}

void WorkTrace::reorder(void)
{
    bool isSwap = true;

    while (isSwap)
    {
        isSwap = false;

        // code statements sink down
        for (size_t i = 0; i < _stmts.size()-1; i++)
        {
            if ( _stmts[i]->getBuoyancy() == Stmt::SINK &&
                 _stmts[i]->getBuoyancy() > _stmts[i+1]->getBuoyancy() &&
                 _stmts[i]->swappable(*_stmts[i+1]) &&
                 _stmts[i+1]->swappable(*_stmts[i]) )
            {
                // swap statements
                Stmt* tmp  = _stmts[i+1];
                _stmts[i+1] = _stmts[i];
                _stmts[i]   = tmp;

                isSwap = true;
            }
        }

        // data rises
        for (size_t i = _stmts.size()-1; i > 0; i--)
        {
            if ( _stmts[i]->getBuoyancy() == Stmt::RISE &&
                 _stmts[i]->getBuoyancy() < _stmts[i-1]->getBuoyancy() &&
                 _stmts[i]->swappable(*_stmts[i-1]) &&
                 _stmts[i-1]->swappable(*_stmts[i]) )
            {
                // swap statements
                Stmt* tmp  = _stmts[i-1];
                _stmts[i-1] = _stmts[i];
                _stmts[i]   = tmp;

                isSwap = true;
            }
        }
    }

    // autotuning rises to the surface
    vector< Stmt* > newStmts;
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        if ((*it)->surfaceBuoyancy())
        {
            newStmts.push_back(*it);
        }
    }

    // everything else is below the surface
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        if (! (*it)->surfaceBuoyancy())
        {
            newStmts.push_back(*it);
        }
    }

    _stmts = newStmts;
}

void WorkTrace::together(void)
{
    vector< Stmt* > newStmts;

    size_t kernelNumber = 0;

    set< uint32_t > candidates;

    for (vector< Stmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        if (! _together.isPush(*it))
        {
            // internal variables have lexical scope within kernel
            const set< uint32_t > ivs = _together.internalVars();
            candidates.insert(ivs.begin(), ivs.end());

            StmtIdSpace* xid = new StmtIdSpace(_together,
                                               kernelNumber++,
                                               _vt);
            xid->setReadoutDim(_traceReadoutDim);

            newStmts.push_back(xid);

            _together.clear();
            _together.isPush(*it);
        }
    }

    if (! _together.stuffInside().empty())
    {
        // internal variables have lexical scope within kernel
        const set< uint32_t > ivs = _together.internalVars();
        candidates.insert(ivs.begin(), ivs.end());

        StmtIdSpace* xid = new StmtIdSpace(_together,
                                           kernelNumber++,
                                           _vt);
        xid->setReadoutDim(_traceReadoutDim);

        newStmts.push_back(xid);
    }

    if (! candidates.empty())
    {
        for (vector< Stmt* >::const_iterator
             it = newStmts.begin();
             it != newStmts.end();
             it++)
        {
            StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

            ElideInternal elideVars(candidates);
            elideVars.edit(*xid);
        }
    }

    set< uint32_t > registerVars;
    set< uint32_t > registerVarsWriteBack;

    for (vector< Stmt* >::const_iterator
         it = newStmts.begin();
         it != newStmts.end();
         it++)
    {
        StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

        UseRegister useReg;
        useReg.edit(*xid);

        registerVars.insert( xid->traceUseRegister().begin(),
                             xid->traceUseRegister().end() );

        registerVarsWriteBack.insert( xid->traceUseRegisterWriteBack().begin(),
                                      xid->traceUseRegisterWriteBack().end() );
    }

    for (vector< Stmt* >::const_iterator
         it = newStmts.begin();
         it != newStmts.end();
         it++)
    {
        StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

        if (0 == xid->streamW() && 0 == xid->streamH())
        {
            // no code, only data movement
            // remove any create data for variables in registers
            TrimFat trimFat(registerVars,
                            registerVarsWriteBack);
            trimFat.edit(*xid);
        }
        else
        {
            // must be code, need to build and run kernels
            for (set< uint32_t >::const_iterator
                 it = registerVars.begin();
                 it != registerVars.end();
                 it++)
            {
                if (! registerVarsWriteBack.count(*it))
                {
                    xid->removeTraceArg(*it);
                }
            }
        }
    }

    _stmts = newStmts;
}

void WorkTrace::traverse(VisitStmt& v)
{
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        (*it)->accept(v);
    }
}

}; // namespace chai_internal
