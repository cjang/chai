// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtIdSpace.hpp"
#include "XStmtMaker.hpp"
#include "XStmtTrace.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// converted statement trace

XStmtTrace::XStmtTrace(BCStmtTrace& trace)
    : _vt(trace.getVectorTrace())
{
    XStmtMaker v(_stmts, _vt);

    trace.traverse(v);

    _traceReadoutDim = v.traceReadoutDim();

    if (v.reductionInsideLoop())
        _together.reductionIsSpecial(false);
}

XStmtTrace::~XStmtTrace(void)
{
    for (vector< XStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        delete *it;
    }
}

VectorTrace& XStmtTrace::getVectorTrace(void)
{
    return _vt;
}

uint64_t XStmtTrace::hashCode(void) const
{
    return _vt.hashCode();
}

size_t XStmtTrace::numTraces(void) const
{
    return _vt.numTraces();
}

void XStmtTrace::reorder(void)
{
    bool isSwap = true;

    while (isSwap)
    {
        isSwap = false;

        // code statements sink down
        for (size_t i = 0; i < _stmts.size()-1; i++)
        {
            if ( _stmts[i]->getBuoyancy() == XStmt::SINK &&
                 _stmts[i]->getBuoyancy() > _stmts[i+1]->getBuoyancy() &&
                 _stmts[i]->swappable(*_stmts[i+1]) &&
                 _stmts[i+1]->swappable(*_stmts[i]) )
            {
                // swap statements
                XStmt* tmp  = _stmts[i+1];
                _stmts[i+1] = _stmts[i];
                _stmts[i]   = tmp;

                isSwap = true;
            }
        }

        // data rises
        for (size_t i = _stmts.size()-1; i > 0; i--)
        {
            if ( _stmts[i]->getBuoyancy() == XStmt::RISE &&
                 _stmts[i]->getBuoyancy() < _stmts[i-1]->getBuoyancy() &&
                 _stmts[i]->swappable(*_stmts[i-1]) &&
                 _stmts[i-1]->swappable(*_stmts[i]) )
            {
                // swap statements
                XStmt* tmp  = _stmts[i-1];
                _stmts[i-1] = _stmts[i];
                _stmts[i]   = tmp;

                isSwap = true;
            }
        }
    }

    // autotuning rises to the surface
    vector< XStmt* > newStmts;
    for (vector< XStmt* >::const_iterator
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
    for (vector< XStmt* >::const_iterator
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

void XStmtTrace::together(void)
{
    vector< XStmt* > newStmts;

    size_t kernelNumber = 0;

    for (vector< XStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        if (! _together.isPush(*it))
        {

            XStmtIdSpace* xid = new XStmtIdSpace(_together,
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

        XStmtIdSpace* xid = new XStmtIdSpace(_together,
                                             kernelNumber++,
                                             _vt);
        xid->setReadoutDim(_traceReadoutDim);

        newStmts.push_back(xid);
    }

    _stmts = newStmts;
}

void XStmtTrace::traverse(VisitXStmt& v)
{
    for (vector< XStmt* >::const_iterator
         it = _stmts.begin();
         it != _stmts.end();
         it++)
    {
        (*it)->accept(v);
    }
}

}; // namespace chai_internal
