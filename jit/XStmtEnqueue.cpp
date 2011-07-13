// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "CodeText.hpp"
#include "Variable.hpp"
#include "XStmtCompound.hpp"
#include "XStmtCreateData.hpp"
#include "XStmtGather.hpp"
#include "XStmtIdSpace.hpp"
#include "XStmtIndex.hpp"
#include "XStmtLiteral.hpp"
#include "XStmtMatmul.hpp"
#include "XStmtEnqueue.hpp"
#include "XStmtReadData.hpp"
#include "XStmtReduce.hpp"
#include "XStmtRepeat.hpp"
#include "XStmtRNGrand.hpp"
#include "XStmtRNGseed.hpp"
#include "XStmtSendData.hpp"
#include "XStmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// enqueue converted trace as OpenCL

XStmtEnqueue::XStmtEnqueue(VectorTrace& vt,
                           MemManager& mm)
    : _vt(vt),
      _memMgr(mm),
      _printer(NULL),
      _os(NULL),
      _failure(false) { }

XStmtEnqueue::XStmtEnqueue(VectorTrace& vt,
                           MemManager& mm,
                           ostream& os)
    : _vt(vt),
      _memMgr(mm),
      _printer(new XStmtPrinter(os)),
      _os(&os),
      _failure(false) { }

XStmtEnqueue::~XStmtEnqueue(void)
{
    delete _printer;
}

bool XStmtEnqueue::isOk(void) const
{
    return ! _failure;
}

////////////////////////////////////////
// VisitXStmt

void XStmtEnqueue::visit(XStmtCompound&)
{
    if (_failure) return;

    _failure = true;
}

void XStmtEnqueue::visit(XStmtCreateData& s)
{
    if (_failure) return;

    if (s.lhsVariable()->isTraceVariable())
    {
        // client threads may reference this variable and swizzle data back

        if (s.initializeData())
        {
            // note all variable declarations reach here

            if (s.lhsVariable()->isDouble())
            {
                if (! _memMgr.createDevMem( _vt,
                                            s.lhsVariable()->variable(),
                                            s.lhsVariable()->W(),
                                            s.lhsVariable()->H(),
                                            s.doubleValue() ))
                _failure = true;
            }
            else
            {
                if (! _memMgr.createDevMem( _vt,
                                            s.lhsVariable()->variable(),
                                            s.lhsVariable()->W(),
                                            s.lhsVariable()->H(),
                                            s.floatValue() ))
                _failure = true;
            }
        }
        else
        {
            // note that only operation splits reach here

            if (! _memMgr.createDevMem( _vt,
                                        s.lhsVariable()->variable(),
                                        s.lhsVariable()->W(),
                                        s.lhsVariable()->H(),
                                        s.lhsVariable()->isDouble() ))
                _failure = true;
        }
    }
    else
    {
        // no issues as client threads do not reference this variable

        if (s.initializeData())
        {
            if (s.lhsVariable()->isDouble())
            {
                if (! _memMgr.createDevMem( _vt,
                                            s.lhsVariable(),
                                            s.lhsVariable()->W(),
                                            s.lhsVariable()->H(),
                                            s.doubleValue() ))
                    _failure = true;
            }
            else
            {
                if (! _memMgr.createDevMem( _vt,
                                            s.lhsVariable(),
                                            s.lhsVariable()->W(),
                                            s.lhsVariable()->H(),
                                            s.floatValue() ))
                    _failure = true;
            }
        }
        else
        {
            if (! _memMgr.createDevMem( _vt,
                                        s.lhsVariable(),
                                        s.lhsVariable()->W(),
                                        s.lhsVariable()->H(),
                                        s.lhsVariable()->isDouble() ))
                _failure = true;
        }
    }
}

void XStmtEnqueue::visit(XStmtGather&)
{
    if (_failure) return;

    _failure = true;
}

void XStmtEnqueue::visit(XStmtIdSpace& idsStmt)
{
/* FIXME - commented out so printed output is not suppressed
    if (_failure) return;
*/

    if (_printer)
    {
        _printer->visit(idsStmt);
        *_os << endl;
    }

    if (0 == idsStmt.indexW() && 0 == idsStmt.indexH())
    {
        // no code, only data movement

        for (vector< XStmt* >::const_iterator
             it = idsStmt.stuffInside().begin();
             it != idsStmt.stuffInside().end();
             it++)
        {
            (*it)->accept(*this);
        }
    }
    else
    {
        // must be code

        // FIXME - not really doing anything here yet
        _failure = true;

        map< Variable*, AstVariable* > kvars;

        for (set< AstVariable* >::const_iterator
             it = idsStmt.allVariable().begin();
             it != idsStmt.allVariable().end();
             it++)
        {
            kvars[new FloatPt((*it)->precision(), 1)] = *it;
        }

        for (map< Variable*, AstVariable* >::const_iterator
             it = kvars.begin();
             it != kvars.end();
             it++)
        {
            declare(*_os, *(*it).first);

            *_os << " is ";

            if ((*it).second->isTraceVariable())
                *_os << "trace variable " << (*it).second->variable();
            else
                *_os << "split variable " << (*it).second;

            *_os << endl;

            delete (*it).first;
        }
    }
}

void XStmtEnqueue::visit(XStmtIndex&)
{
    if (_failure) return;

    _failure = true;
}

void XStmtEnqueue::visit(XStmtLiteral&)
{
    if (_failure) return;

    _failure = true;
}

void XStmtEnqueue::visit(XStmtMatmul&)
{
    if (_failure) return;

    _failure = true;
}

void XStmtEnqueue::visit(XStmtReadData& s)
{
    if (_failure) return;

    s.readoutPtr()->backMem()->swizzle();
}

void XStmtEnqueue::visit(XStmtReduce&)
{
    if (_failure) return;

    _failure = true;
}

void XStmtEnqueue::visit(XStmtRepeat&)
{
    if (_failure) return;

    _failure = true;
}

void XStmtEnqueue::visit(XStmtRNGrand&)
{
    if (_failure) return;

    _failure = true;
}

void XStmtEnqueue::visit(XStmtRNGseed&)
{
    if (_failure) return;

    _failure = true;

    *_os << "XStmtRNGseed" << endl;
}

void XStmtEnqueue::visit(XStmtSendData& s)
{
    if (_failure) return;

    if (s.arrayMemPtr())
    {
        if (s.lhsVariable()->isTraceVariable())
        {
            // client threads may reference this variable and swizzle data back
            if (! _memMgr.createDevMem( _vt,
                                        s.lhsVariable()->variable(),
                                        s.arrayMemPtr()->backMem(),
                                        s.lhsVariable()->isReadOnly() ))
                _failure = true;
        }
        else
        {
            // no issues as client threads do not reference this variable
            if (! _memMgr.createDevMem( _vt,
                                        s.lhsVariable(),
                                        s.arrayMemPtr()->backMem(),
                                        s.lhsVariable()->isReadOnly() ))
                _failure = true;
        }
    }

    if (s.makedataPtr())
    {
        if (s.lhsVariable()->isTraceVariable())
        {
            // client threads may reference this variable and swizzle data back
            if (! _memMgr.createDevMem( _vt,
                                        s.lhsVariable()->variable(),
                                        s.makedataPtr()->backMem(),
                                        s.lhsVariable()->isReadOnly() ))
                _failure = true;
        }
        else
        {
            // no issues as client threads do not reference this variable\
            // (can this case ever happen?)
            if (! _memMgr.createDevMem( _vt,
                                        s.lhsVariable(),
                                        s.makedataPtr()->backMem(),
                                        s.lhsVariable()->isReadOnly() ))
                _failure = true;
        }
    }
}

void XStmtEnqueue::visit(XStmtSingle&)
{
    if (_failure) return;

    _failure = true;
}

}; // namespace chai_internal
