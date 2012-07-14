// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include <set>

#include "ElideInternal.hpp"
#include "Logger.hpp"
#include "MakeStmt.hpp"
//FIXME #include "NestRNG.hpp"
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
         it = _stmts.begin(); it != _stmts.end(); it++)
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
         it = _stmts.begin(); it != _stmts.end(); it++)
    {
        if ((*it)->surfaceBuoyancy())
        {
            newStmts.push_back(*it);
        }
    }

    // everything else is below the surface
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin(); it != _stmts.end(); it++)
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

    //
    // group statements into kernels and track internal variables
    //

    for (vector< Stmt* >::const_iterator
         it = _stmts.begin(); it != _stmts.end(); it++)
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

#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "candidate internal trace variables:";
    for (set< uint32_t >::const_iterator
         it = candidates.begin(); it != candidates.end(); it++)
    {
        ss << " " << (*it);
    }
    LOGGER(ss.str())
#endif

    //
    // elide variables with lexical scope inside a kernel
    //

    if (! candidates.empty())
    {
        for (vector< Stmt* >::const_iterator
             it = newStmts.begin(); it != newStmts.end(); it++)
        {
            StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

            ElideInternal elideVars(candidates);
            elideVars.edit(*xid);
        }
    }

    //
    // use private register variables instead of global memory
    //

    set< uint32_t > traceRegisterVars;
    set< uint32_t > traceRegisterVarsWriteBack;

    for (vector< Stmt* >::const_iterator
         it = newStmts.begin(); it != newStmts.end(); it++)
    {
        StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

        UseRegister useReg;
        useReg.edit(*xid);

        traceRegisterVars.insert(
            xid->traceUseRegister().begin(),
            xid->traceUseRegister().end() );

        traceRegisterVarsWriteBack.insert(
            xid->traceUseRegisterWriteBack().begin(),
            xid->traceUseRegisterWriteBack().end() );
    }

#ifdef __LOGGING_ENABLED__
    ss.str(string());
    ss << "register variables:";
    for (set< uint32_t >::const_iterator
         it = traceRegisterVars.begin(); it != traceRegisterVars.end(); it++)
    {
        ss << " " << (*it);
    }
    ss << " write back:";
    for (set< uint32_t >::const_iterator
         it = traceRegisterVarsWriteBack.begin();
         it != traceRegisterVarsWriteBack.end();
         it++)
    {
        ss << " " << (*it);
    }
    LOGGER(ss.str())
#endif

    //
    // a register variable has no global memory or kernel argument
    //

    for (vector< Stmt* >::const_iterator
         it = newStmts.begin(); it != newStmts.end(); it++)
    {
        StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

        if (0 == xid->streamW() && 0 == xid->streamH())
        {
            // no code, only data movement

            // remove any create data for variables in registers
            TrimFat trimFat(traceRegisterVars,
                            traceRegisterVarsWriteBack);

            trimFat.edit(*xid);
        }
        else
        {
            // must be code, need to build and run kernels

            for (set< uint32_t >::const_iterator
                 it = traceRegisterVars.begin();
                 it != traceRegisterVars.end();
                 it++)
            {
                if (! traceRegisterVarsWriteBack.count(*it))
                    xid->removeTraceArg(*it);
            }
        }
    }

/*FIXME
    //
    // nest RNG inside reduction loops
    //

//    if (! candidates.empty())
//    {
        for (vector< Stmt* >::const_iterator
             it = newStmts.begin(); it != newStmts.end(); it++)
        {
            StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

            NestRNG nestRNG(candidates);
            nestRNG.edit(*xid);
        }
//    }

    //
    // elide variables with lexical scope inside a kernel
    //

    if (! candidates.empty())
    {
        for (vector< Stmt* >::const_iterator
             it = newStmts.begin(); it != newStmts.end(); it++)
        {
            StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

            ElideInternal elideVars(candidates);
            elideVars.edit(*xid);
        }
    }

    //
    // use private register variables instead of global memory
    //

    for (vector< Stmt* >::const_iterator
         it = newStmts.begin(); it != newStmts.end(); it++)
    {
        StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

        UseRegister useReg;
        useReg.edit(*xid);

        traceRegisterVars.insert(
            xid->traceUseRegister().begin(),
            xid->traceUseRegister().end() );

        traceRegisterVarsWriteBack.insert(
            xid->traceUseRegisterWriteBack().begin(),
            xid->traceUseRegisterWriteBack().end() );
    }

    //
    // a register variable has no global memory or kernel argument
    //

    for (vector< Stmt* >::const_iterator
         it = newStmts.begin(); it != newStmts.end(); it++)
    {
        StmtIdSpace* xid = static_cast< StmtIdSpace* >(*it);

        if (0 == xid->streamW() && 0 == xid->streamH())
        {
            // no code, only data movement

            // remove any create data for variables in registers
            TrimFat trimFat(traceRegisterVars,
                            traceRegisterVarsWriteBack);

            trimFat.edit(*xid);
        }
        else
        {
            // must be code, need to build and run kernels

            for (set< uint32_t >::const_iterator
                 it = traceRegisterVars.begin();
                 it != traceRegisterVars.end();
                 it++)
            {
                if (! traceRegisterVarsWriteBack.count(*it))
                    xid->removeTraceArg(*it);
            }
        }
    }
*/

    _stmts = newStmts;
}

void WorkTrace::traverse(VisitStmt& v)
{
    for (vector< Stmt* >::const_iterator
         it = _stmts.begin(); it != _stmts.end(); it++)
    {
        (*it)->accept(v);
    }
}

}; // namespace chai_internal
