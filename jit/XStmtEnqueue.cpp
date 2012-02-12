// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <set>
#include <sstream>

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstExtension.hpp"
#include "AstFun1.hpp"
#include "AstFun2.hpp"
#include "AstFun3.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstLitdata.hpp"
#include "AstMakedata.hpp"
#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulVV.hpp"
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstTranspose.hpp"
#include "AstVariable.hpp"
#include "AutoTuneBench.hpp"
#include "AutoTuneEM.hpp"
#include "AutoTuneMemo.hpp"
#include "BaseAst.hpp"
#include "EvergreenMatmulBase.hpp"
#include "EvergreenMatmulMM.hpp"
#include "EvergreenMatmulMV.hpp"
#include "JustInTimeMemo.hpp"
#include "Logger.hpp"
#include "OCLdevice.hpp"
#include "OCLhacks.hpp"
#include "StandardEM.hpp"
#include "Variable.hpp"
#include "XStmtBarrier.hpp"
#include "XStmtCompound.hpp"
#include "XStmtCreateData.hpp"
#include "XStmtEnqueue.hpp"
#include "XStmtExtension.hpp"
#include "XStmtExtensionAuto.hpp"
#include "XStmtIdSpace.hpp"
#include "XStmtIndex.hpp"
#include "XStmtLiteral.hpp"
#include "XStmtMatmul.hpp"
#include "XStmtMatmulAuto.hpp"
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

void XStmtEnqueue::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

void XStmtEnqueue::boundVectorLength(void)
{
    size_t minlen = 4; // largest possible
    size_t maxlen = 1; // smallest possible

    for (set< uint32_t >::const_iterator
         it = _xid->traceArgs().begin();
         it != _xid->traceArgs().end();
         it++)
    {
        const uint32_t varNum = *it;

        size_t vlen = getVectorLength(varNum);

        if (0 == vlen)
        {
            const size_t prec = _xid->getPrecision(varNum);
            if (sizeof(double) == prec)
                vlen = 2; // double2
            else
                vlen = 4; // float4
        }

        if (vlen < minlen) minlen = vlen;
        if (vlen > maxlen) maxlen = vlen;
    }

    for (set< const AstVariable* >::const_iterator
         it = _xid->splitArgs().begin();
         it != _xid->splitArgs().end();
         it++)
    {
        const AstVariable* varPtr = *it;

        size_t vlen = getVectorLength(varPtr);

        if (0 == vlen)
        {
            const size_t prec = _xid->getPrecision(varPtr);
            if (sizeof(double) == prec)
                vlen = 2; // double2
            else
                vlen = 4; // float4
        }

        if (vlen < minlen) minlen = vlen;
        if (vlen > maxlen) maxlen = vlen;
    }

    _minVectorLength = minlen;
    _maxVectorLength = maxlen;
}

size_t XStmtEnqueue::getVectorLength(const uint32_t varNum)
{
    const size_t vlen = _jitMemo.getVectorLength(
                                     varNum,
                                     _xid->getPrecision(varNum) );

    // apply transposed array subscript constraint to memory buffers only
    return (0 != vlen && (_xid->anyTransposed() || _xid->anyGathered()))
               ? 1
               : vlen;
}

size_t XStmtEnqueue::getVectorLength(const AstVariable* varPtr)
{
    const size_t vlen = _jitMemo.getVectorLength( varPtr );

    // apply transposed array subscript constraint to memory buffers only
    return (0 != vlen && (_xid->anyTransposed() || _xid->anyGathered()))
               ? 1
               : vlen;
}

XStmtEnqueue::XStmtEnqueue(VectorTrace& vt,
                           MemManager& mm,
                           StandardEM& stdEM,
                           JustInTimeMemo& jitMemo)
    : _vt(vt),
      _memMgr(mm),
      _stdEM(stdEM),
      _jitMemo(jitMemo),
      _printer(NULL),
      _xid(NULL),
      _failure(false),
      _func(NULL),
      _scalarToScalar(false)
{
#ifdef __LOGGING_ENABLED__
    _printer = new XStmtPrinter(cerr);
#endif
}

XStmtEnqueue::~XStmtEnqueue(void)
{
    delete _printer;
}

bool XStmtEnqueue::isOk(void) const
{
    return ! _failure;
}

////////////////////////////////////////
// VisitAst

void XStmtEnqueue::visit(AstAccum& v)
{
    // reduction statement requires multiple statements
    // AST object is used for the statement inside the accumulation loop
    // note nested reductions are impossible due to statement splits
    descendAst(v);
}

void XStmtEnqueue::visit(AstArrayMem& v)
{
    // split off as new statement and variable earlier so nothing here

    // case of direct assignment to a LHS variable is handled earlier by
    // replacing the RHS AST array memory object with a variable
}

void XStmtEnqueue::visit(AstCond& v)
{
    // really this is like the C conditional ternary operator
    _rhsStmt << "(";
    v.getArg(0)->accept(*this); // predicate
    _rhsStmt << ") ? (";
    v.getArg(1)->accept(*this); // argument if predicate is true
    _rhsStmt << ") : (";
    v.getArg(2)->accept(*this); // argument if predicate is false
    _rhsStmt << ")";
}

void XStmtEnqueue::visit(AstConvert& v)
{
    // AST convert object is a NOP in the JIT
    // interpreter needs type of LHS for mixed precision assignment
    descendAst(v);
}

void XStmtEnqueue::visit(AstDotprod& v)
{
    // reduction statement requires multiple statements
    // AST object is used for the statement inside the accumulation loop
    // note nested reductions are impossible due to statement splits
    _rhsStmt << "(";
    v.getArg(0)->accept(*this); // left argument
    _rhsStmt << ") * (";
    v.getArg(1)->accept(*this); // right argument
    _rhsStmt << ")";
}

void XStmtEnqueue::visit(AstExtension& v)
{
    // split off as separate statement earlier
}

void XStmtEnqueue::visit(AstFun1& v)
{
    // function call with one argument
    _rhsStmt << v.fun().str() << "(";
    v.getArg(0)->accept(*this); // argument
    _rhsStmt << ")";
}

void XStmtEnqueue::visit(AstFun2& v)
{
    if (v.fun().infix()) // overloaded operator with two arguments
    {
        _rhsStmt << "(";
        v.getArg(0)->accept(*this); // left
        _rhsStmt << " "
                 << v.fun().str()
                 << " ";
        v.getArg(1)->accept(*this); // right
        _rhsStmt << ")";
    }
    else // function call with two arguments
    {
        _rhsStmt << v.fun().str() << "(";
        v.getArg(0)->accept(*this); // left
        _rhsStmt << ", ";
        v.getArg(1)->accept(*this); // right
        _rhsStmt << ")";
    }
}

void XStmtEnqueue::visit(AstFun3& v)
{
    // function call with three arguments
    _rhsStmt << v.fun().str() << "(";
    v.getArg(0)->accept(*this); // left
    _rhsStmt << ", ";
    v.getArg(1)->accept(*this); // middle
    _rhsStmt << ", ";
    v.getArg(2)->accept(*this); // right
    _rhsStmt << ")";
}

void XStmtEnqueue::visit(AstGather& v)
{
    // save RHS statement so far
    const string saveRHS = _rhsStmt.str();

    // width index
    _rhsStmt.str(string());
    v.getArg(1)->accept(*this);
    _subs.top()->setGatherWidthIndex(_rhsStmt.str());

    // height index
    if (2 == v.N())
    {
        _rhsStmt.str(string());
        v.getArg(2)->accept(*this);
        _subs.top()->setGatherHeightIndex(_rhsStmt.str());
    }

    // restore RHS
    _rhsStmt.str(string());
    _rhsStmt << saveRHS;

    // gather data using explicit width and height subscript
    v.getArg(0)->accept(*this);

    // restore the subscript to normal
    _subs.top()->unsetGatherIndex();
}

void XStmtEnqueue::visit(AstIdxdata& v)
{
    _subs.top()->setVariable( v.W(),
                              v.H(),
                              _lhsVectorLength );

    switch (_lhsVectorLength)
    {
        case (1) :
            if (0 == v.index())
            {
                _subs.top()->widthIdx(_rhsStmt);
            }
            else
            {
                _subs.top()->disableBlocking();
                _subs.top()->heightIdx(_rhsStmt);
                _subs.top()->enableBlocking();
            }
            break;

        case (2) :
            _rhsStmt << "(" << (v.isDouble() ? "double" : "float") << "2)(";
            if (0 == v.index())
            {
                stringstream ss;
                _subs.top()->widthIdx(ss);
                ss << " * " << _lhsVectorLength;
                _rhsStmt << ss.str() << ", " << ss.str() << " + 1";
            }
            else
            {
                _subs.top()->disableBlocking();
                _subs.top()->heightIdx(_rhsStmt);
                _subs.top()->enableBlocking();
            }
            _rhsStmt << ")";
            break;

        case (4) :
            _rhsStmt << "(" << (v.isDouble() ? "double" : "float") << "4)(";
            if (0 == v.index())
            {
                stringstream ss;
                _subs.top()->widthIdx(ss);
                ss << " * " << _lhsVectorLength;
                _rhsStmt << ss.str() << ", "
                         << ss.str() << " + 1,"
                         << ss.str() << " + 2,"
                         << ss.str() << " + 3";
            }
            else
            {
                _subs.top()->disableBlocking();
                _subs.top()->heightIdx(_rhsStmt);
                _subs.top()->enableBlocking();
            }
            _rhsStmt << ")";
            break;
    }
}

void XStmtEnqueue::visit(AstLitdata& v)
{
    // split off as separate statement earlier
}

void XStmtEnqueue::visit(AstMakedata& v)
{
    // split off as separate statement earlier
}

void XStmtEnqueue::visit(AstMatmulMM& v)
{
    // split off as separate kernel earlier
}

void XStmtEnqueue::visit(AstMatmulMV& v)
{
    // split off as separate kernel earlier
}

void XStmtEnqueue::visit(AstMatmulVM& v)
{
    // split off as separate kernel earlier
}

void XStmtEnqueue::visit(AstMatmulVV& v)
{
    _rhsStmt << "(";

    _subs.top()->setOuterProductLeft();
    v.getArg(0)->accept(*this); // left argument

    _rhsStmt << ") * (";

    _subs.top()->setOuterProductRight();
    v.getArg(1)->accept(*this); // right argument

    _rhsStmt << ")";

    _subs.top()->unsetOuterProduct();
}

void XStmtEnqueue::visit(AstReadout& v)
{
    // split off as separate statement earlier
}

void XStmtEnqueue::visit(AstRNGnormal& v)
{
    // FIXME - RNG support is TBD
}

void XStmtEnqueue::visit(AstRNGuniform& v)
{
    // FIXME - RNG support is TBD
}

void XStmtEnqueue::visit(AstScalar& v)
{
    const double val = v.isDouble()
                           ? v.doubleValue()
                           : v.floatValue();

    _rhsStmt << val;
}

void XStmtEnqueue::visit(AstTranspose& v)
{
    // transpose the index space
    _subs.top()->setTranspose();

    descendAst(v);

    // un-transpose the index space
    _subs.top()->unsetTranspose();
}

void XStmtEnqueue::visit(AstVariable& v)
{
    Variable* funcVar = v.isTraceVariable()
                            ? _func->traceVar(v.variable())
                            : _func->splitVar(&v);

    // same data across traces suppresses data tile part of subscript
    bool suppressTile = false;
    if (! _func->isPrivate(funcVar) && v.isTraceVariable())
    {
        const vector< size_t > frontIndex
                                   = _vt.memallocFrontIndex( v.variable() );

        set< size_t > frontSet;

        for (vector< size_t >::const_iterator
             it = frontIndex.begin();
             it != frontIndex.end();
             it++)
        {
            frontSet.insert(*it);
        }

        if (1 == frontSet.size())
        {
            suppressTile = true;
        }
    }

    const size_t vecLen = getVectorLength(&v);

    // standard vector length is double2 and float4
    const size_t standardVectorLength = v.isDouble() ? 2 : 4;

    // special case for scalar reductions
    // relies on implicit convention of array dimensions
    const bool specialCaseReadScalar = 1 == v.H() && 1 == v.W();

    // really the same hack as everywhere else for scalars, need to sum
    // vector elements for RHS 1x1 variables due to standard vector length
    // (note: only applying hack for global memory buffers)
    string reducePrefix, reduceSuffix;
    if (specialCaseReadScalar && 1 != vecLen)
    {
        reducePrefix = (2 == standardVectorLength)
                           ? "dot((double2)(1.0f), "
                           : "dot((float4)(1.0f), ";

        reduceSuffix = ")";
    }

    if (0 == vecLen) // image
    {
        const size_t effVecLen = v.isDouble() ? 2 : 4;

        _subs.top()->setVariable( v.W(), v.H(), effVecLen );
        if (_subs.top()->isMixedZero())
        {
            _rhsStmt << "0";
        }
        else
        {
            stringstream ss;
            ss << (v.isDouble() ? "as_double2(read_imageui(" : "read_imagef(");
            funcVar->identifierName(ss);
            ss << ", sampler, (int2)(";
            _subs.top()->widthIdx(ss);
            ss << ", ";
            if (suppressTile) _subs.top()->disableBlocking();
            _subs.top()->heightIdx(ss);
            if (suppressTile) _subs.top()->enableBlocking();
            ss << (v.isDouble() ? ")))" : "))");

            if (_subs.top()->isMixedVectorLength())
            {
                FloatPt* tempVar = _func->getPrivateVar( v.precision(),
                                                         effVecLen );
                _func->assignStatement(tempVar, ss.str());

                tempVar->identifierName(_rhsStmt);
                _subs.top()->mixedComponentSub(_rhsStmt);
            }
            else
            {
                _rhsStmt << ss.str();
            }
        }
    }
    else // memory buffer
    {
        if (_func->isPrivate(funcVar))
        {
            // private variables are scalars/vectors (but not arrays)
            funcVar->identifierName(_rhsStmt); // print variable identifier
        }
        else
        {
            // global variables are arrays and require a subscript
            _subs.top()->setVariable( v.W(), v.H(), vecLen );
            if (_subs.top()->isMixedZero())
            {
                _rhsStmt << "0";
            }
            else
            {
                if (! _scalarToScalar) _rhsStmt << reducePrefix;

                funcVar->identifierName(_rhsStmt); // print variable identifier

                if (suppressTile) _subs.top()->disableBlocking();
                _subs.top()->arraySub(_rhsStmt); // print array subscript
                if (suppressTile) _subs.top()->enableBlocking();

                if (! _scalarToScalar) _rhsStmt << reduceSuffix;
            }
        }
    }
}

////////////////////////////////////////
// VisitXStmt

void XStmtEnqueue::visit(XStmtBarrier& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    _func->barrier();
}

void XStmtEnqueue::visit(XStmtCompound& s)
{
    for (vector< XStmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        if (_failure) return;

        (*it)->accept(*this);
    }
}

void XStmtEnqueue::visit(XStmtCreateData& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    const AstVariable* varPtr = s.lhsVariable();
    const bool     isDP       = varPtr->isDouble();

    const uint32_t varNum = s.lhsVariable()->variable();
    const size_t   prec   = s.lhsVariable()->precision();
    const float    val32  = s.floatValue();
    const double   val64  = s.doubleValue();

    if (s.outerLexicalScope())
    {
        // variable is in outermost lexical scope
        // therefore, it may span kernels or be referenced by clients

        if (s.initializeData())
        {
            // note all variable declarations reach here
            if (isDP)
            {
                if (! _jitMemo.createArrayTemp(varPtr, *_xid, val64))
                    _failure = true;
            }
            else
            {
                if (! _jitMemo.createArrayTemp(varPtr, *_xid, val32))
                    _failure = true;
            }
        }
        else
        {
            // note that only operation splits reach here
            if (! _jitMemo.createArrayTemp(varPtr, *_xid))
                _failure = true;
        }
    }
    else
    {
        // variable is inside a for-loop, therefore it is private

        size_t maxVecLen = _minVectorLength;
        for (set< AstVariable* >::const_iterator
             it = s.underlyingVars().begin();
             it != s.underlyingVars().end();
             it++)
        {
            const size_t vecLen = getVectorLength(*it);
            if (vecLen > maxVecLen)
                maxVecLen = vecLen;
        }

        FloatPt* newVar
            = varPtr->isTraceVariable()
                  ? _func->createPrivate(varNum, prec, maxVecLen)
                  : _func->createPrivate(varPtr, prec, maxVecLen);

        if (s.initializeData())
        {
            _func->assignNumber( newVar,
                                 (isDP ? val64 : val32) );
        }
    }
}

void XStmtEnqueue::visit(XStmtExtension& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    if (! s.extensionEnqueue(_memMgr, _vt))
        _failure = true;
}

void XStmtEnqueue::visit(XStmtExtensionAuto& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    if (! s.extensionEnqueue(_memMgr, _vt))
        _failure = true;
}

void XStmtEnqueue::visit(XStmtIdSpace& s)
{
    if (_failure) return;

    // set containing context of ID space
    _xid = &s;

    // stream dimensions distinguish between code and data movement
    if (0 == s.streamW() && 0 == s.streamH())
    {
        // no code, only data movement

        for (vector< XStmt* >::const_iterator
             it = s.stuffInside().begin();
             it != s.stuffInside().end();
             it++)
        {
            (*it)->accept(*this);
        }
    }
    else
    {
        // must be code, need to build and run kernels

        if (XStmtIdSpace::NOTHING_SPECIAL == s.specialK())
        {
            // create custom kernel function
            _func = new Function(_memMgr.getDeviceIndex(),
                                 s.hashCode(),
                                 s.kernelNumber());

            // trace variable arguments
            for (set< uint32_t >::const_iterator
                 it = s.traceArgs().begin();
                 it != s.traceArgs().end();
                 it++)
            {
                const uint32_t varNum = *it;
                const size_t   prec   = s.getPrecision(varNum);
                const size_t   vlen   = getVectorLength(varNum);

                if (s.isReadWrite(varNum))
                {
                    if (0 != vlen)
                        _func->addArgument(
                            varNum,
                            new FloatPt(prec, vlen, POINTER, GLOBAL) );
                }
                else if (s.isReadOnly(varNum))
                {
                    if (0 == vlen)
                        _func->addArgument(
                            varNum,
                            new Image2D(READONLY) );
                    else
                        _func->addArgument(
                            varNum,
                            new FloatPt(prec, vlen, CONST_POINTER, GLOBAL) );
                }
                else if (s.isWriteOnly(varNum))
                {
                    if (0 == vlen)
                        _func->addArgument(
                            varNum,
                            new Image2D(WRITEONLY) );
                    else
                        _func->addArgument(
                            varNum,
                            new FloatPt(prec, vlen, POINTER, GLOBAL) );
                }
            }

            // split variable arguments
            for (set< const AstVariable*, bool >::const_iterator
                 it = s.splitArgs().begin();
                 it != s.splitArgs().end();
                 it++)
            {
                const AstVariable* varPtr = *it;
                const size_t       prec   = s.getPrecision(varPtr);
                const size_t       vlen   = getVectorLength(varPtr);

                if (s.isReadWrite(varPtr))
                {
                    if (0 != vlen)
                        _func->addArgument(
                            varPtr,
                            new FloatPt(prec, vlen, POINTER, GLOBAL) );
                }
                else if (s.isReadOnly(varPtr))
                {
                    if (0 == vlen)
                        _func->addArgument(
                            varPtr,
                            new Image2D(READONLY) );
                    else
                        _func->addArgument(
                            varPtr,
                            new FloatPt(prec, vlen, CONST_POINTER, GLOBAL) );
                }
                else if (s.isWriteOnly(varPtr))
                {
                    if (0 == vlen)
                        _func->addArgument(
                            varPtr,
                            new Image2D(WRITEONLY) );
                    else
                        _func->addArgument(
                            varPtr,
                            new FloatPt(prec, vlen, POINTER, GLOBAL) );
                }
            }

            // min and max variable effective vector lengths (images = 2|4)
            boundVectorLength();

            // data blocking between index space and outer loop
            Subscript* newSub;
            const bool scalarReduction = 1 == s.streamW() && 1 == s.streamH();
            const bool globalIndexOverThreads
                = s.numTraces() > ( scalarReduction
                                        ? 1
                                        : (s.streamW() * s.streamH())
                                          / _minVectorLength );
            if (globalIndexOverThreads)
            {
                // global index space is 1D over thread traces
                // no outer loop but stream assignments have 2D for-loops

                if ( scalarReduction ||
                     s.streamW() * s.streamH() == _minVectorLength )
                {
                    // stream size is a single vector element
                    newSub = new FixedSubscript(0);
                }
                else
                {
                    // stream has width and height
                    newSub = new ForLoopSubscript(
                                     s.streamW() / _minVectorLength,
                                     s.streamH() );
                }

                newSub->setGlobalIndexTiles();
            }
            else
            {
                // global index space is 2D over the array stream
                // outer loop is over thread traces

                // check for case of kernel with 1x1 scalar reduction
                if ( scalarReduction ||
                     s.streamW() * s.streamH() == _minVectorLength )
                {
                    newSub = new FixedSubscript(0);
                }
                else
                {
                    // assume width is at least the minimum vector length
                    newSub = new StreamSubscript(
                                     s.streamW() / _minVectorLength,
                                     s.streamH() );
                }

                // outer loop is over threads
                if (1 != s.numTraces())
                {
                    // there is more than one thread
                    newSub->setOuterLoopTiles(
                                _func->loopBegin(
                                           0,
                                           s.numTraces() ));
                }
            }
            _subs.push(newSub);

            // body of function
            for (vector< XStmt* >::const_iterator
                 it = s.stuffInside().begin();
                 it != s.stuffInside().end();
                 it++)
            {
                (*it)->accept(*this);
            }

            // some compute devices are single precision only
            if ( _func->isFP64() &&
                 !OCLhacks::singleton().supportFP64(_memMgr.getDeviceIndex()) )
            {
                _failure = true;
            }
            else
            {
                // ok, enqueue the kernel

                if (globalIndexOverThreads)
                {
                    // global index space is 1D over thread traces
                    // no outer loop, every stream assignment has a 2D for-loop
                    if (! _memMgr.enqueueKernel( _vt,
                                                 *_func,
                                                 s.numTraces() ))
                        _failure = true;
                }
                else
                {
                    // outer loop is over threads
                    if (1 != s.numTraces()) _func->loopEnd();

                    const size_t effwidth
                        = scalarReduction
                              ? s.streamW()
                              : s.streamW() / _minVectorLength;

                    // global index space is 2D over the array stream
                    // outer loop is over thread traces
                    if (! _memMgr.enqueueKernel( _vt,
                                                 *_func,
                                                 effwidth,
                                                 s.streamH() ))
                        _failure = true;
                }
            }

            // release subscript object
            delete _subs.top();
            _subs.pop();

            // release kernel function
            delete _func;
            _func = NULL; // unnecessary but may be useful for detecting bugs
        }
        else if (XStmtIdSpace::SPECIAL_MATMUL == s.specialK())
        {
            // body of function
            for (vector< XStmt* >::const_iterator
                 it = s.stuffInside().begin();
                 it != s.stuffInside().end();
                 it++)
            {
                (*it)->accept(*this);
            }
        }
        else if (XStmtIdSpace::SPECIAL_MATMUL_AUTO == s.specialK())
        {
            // body of function
            for (vector< XStmt* >::const_iterator
                 it = s.stuffInside().begin();
                 it != s.stuffInside().end();
                 it++)
            {
                (*it)->accept(*this);
            }
        }
    }

    // clear containing context of ID space
    _xid = NULL;
}

void XStmtEnqueue::visit(XStmtIndex& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // LHS variable must be a memory buffer. It must be writable. The only
    // way it could be an image is if it were also an argument to matmul().
    // Then is would also appear on the RHS. So it must also be readable.
    // As it must be readable and writable, it can not be an image. This
    // implies the vector length can never be zero.
    _lhsVectorLength = getVectorLength(s.lhsVariable());

    // stream dimensions come from the left hand side
    const size_t lhsWidth = s.lhsVariable()->W();
    const size_t lhsHeight = s.lhsVariable()->H();

    // if context is the global index space, then every statement with a
    // stream larger than one element needs a loop
    const bool everyStmtNeedsLoop
        = lhsWidth * lhsHeight > _lhsVectorLength;

    // check if a loop is necessary over the stream in the statement
    const bool needLoop = everyStmtNeedsLoop;

    // new subscript for loop
    ForLoopSubscript newSub( lhsWidth / _lhsVectorLength,
                             lhsHeight );

    // is a loop 1D or 2D?
    const bool useLoopH = 1 != lhsHeight;

    // begin loop
    if (needLoop)
    {
        // for-loop index for height
        const size_t loopIdxH = useLoopH
                                    ? _func->loopBegin(
                                                 0,
                                                 lhsHeight )
                                    : -1;

        // for-loop index for width
        const size_t loopIdxW = _func->loopBegin(
                                           0,
                                           lhsWidth < _lhsVectorLength
                                              ? 1
                                              : lhsWidth / _lhsVectorLength );

        // for-loop subscript
        newSub.setLoopVars(loopIdxW, loopIdxH);
        newSub.setGlobalIndexTiles();

        _subs.push(&newSub);
    }

    // LHS variable dimensions in subscript for index arithmetic
    _subs.top()->setVariable( lhsWidth,
                              lhsHeight,
                              _lhsVectorLength );

    // descend into the AST index object
    s.idxdataPtr()->accept(*this);

    // get LHS variable (kernel generation variable, not the AST variable)
    Variable* lhsVar = s.lhsVariable()->isTraceVariable()
                           ? _func->traceVar(s.lhsVariable()->variable())
                           : _func->splitVar(s.lhsVariable());

    // assign LHS with index value
    _func->assignStatement(lhsVar, _subs.top(), _rhsStmt.str());

    // end loop
    if (needLoop)
    {
        if (useLoopH) _func->loopEnd();
        _func->loopEnd();
        _subs.pop();
    }
}

void XStmtEnqueue::visit(XStmtLiteral& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // LHS variable must be a memory buffer. It must be writable. The only
    // way it could be an image is if it were also an argument to matmul().
    // Then is would also appear on the RHS. So it must also be readable.
    // As it must be readable and writable, it can not be an image. This
    // implies the vector length can never be zero.
    _lhsVectorLength = getVectorLength(s.lhsVariable());

    // stream dimensions come from the left hand side
    const size_t lhsWidth = s.lhsVariable()->W();
    const size_t lhsHeight = s.lhsVariable()->H();

    // if context is the global index space, then every statement with a
    // stream larger than one element needs a loop
    const bool everyStmtNeedsLoop
        = _subs.top()->isGlobalIndexTiles() &&
          lhsWidth * lhsHeight > _lhsVectorLength;

    // check if a loop is necessary over the stream in the statement
    const bool needLoop = everyStmtNeedsLoop;

    // new subscript for loop
    ForLoopSubscript newSub( lhsWidth / _lhsVectorLength,
                             lhsHeight );

    // is a loop 1D or 2D?
    const bool useLoopH = 1 != lhsHeight;

    // begin loop
    if (needLoop)
    {
        // for-loop index for height
        const size_t loopIdxH = useLoopH
                                    ? _func->loopBegin(
                                                 0,
                                                 lhsHeight )
                                    : -1;

        // for-loop index for width
        const size_t loopIdxW = _func->loopBegin(
                                           0,
                                           lhsWidth < _lhsVectorLength
                                              ? 1
                                              : lhsWidth / _lhsVectorLength );

        // for-loop subscript
        newSub.setLoopVars(loopIdxW, loopIdxH);
        newSub.setGlobalIndexTiles();

        _subs.push(&newSub);
    }

    // LHS variable dimensions in subscript for index arithmetic
    _subs.top()->setVariable( lhsWidth,
                              lhsHeight,
                              getVectorLength(s.lhsVariable()) );

    // get literal value to assign
    double rhsVal;
    if (s.litdataPtr())
    {
        rhsVal = s.litdataPtr()->isDouble()
                     ? s.litdataPtr()->doubleValue()
                     : s.litdataPtr()->floatValue();
    }
    if (s.scalarPtr())
    {
        rhsVal = s.scalarPtr()->isDouble()
                     ? s.scalarPtr()->doubleValue()
                     : s.scalarPtr()->floatValue();
    }

    // get LHS variable (kernel generation variable, not the AST variable)
    Variable* lhsVar = s.lhsVariable()->isTraceVariable()
                           ? _func->traceVar(s.lhsVariable()->variable())
                           : _func->splitVar(s.lhsVariable());

    // add new assignment statement
    _func->assignNumber(lhsVar, _subs.top(), rhsVal);

    // end loop
    if (needLoop)
    {
        if (useLoopH) _func->loopEnd();
        _func->loopEnd();
        _subs.pop();
    }
}

void XStmtEnqueue::visit(XStmtMatmul& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // some compute devices are single precision only
    if (! OCLhacks::singleton().supportFP64(_memMgr.getDeviceIndex()) &&
        (sizeof(double) == s.precisionA() ||
         sizeof(double) == s.precisionB() ||
         sizeof(double) == s.precisionC()) )
    {
        _failure = true;
        return;
    }

    if (s.isMATMUL())
    {
        // check if device supports the Evergreen matrix multiply
        if (OCLhacks::singleton().supportEvergreen(_memMgr.getDeviceIndex()))
        {
            // do not allow modulo stream array subscripting for K dimension
            const size_t KfromA = s.isTransposeA() ? s.heightA() : s.widthA();
            const size_t KfromB = s.isTransposeB() ? s.widthB() : s.heightB();
            if (KfromA != KfromB)
            {
                _failure = true;
                return;
            }

            // ATI Evergreen matrix multiply
            Evergreen::MatmulMM matmul(_memMgr.getDeviceIndex());

            // exogenous parameters
            matmul.setPacking(_vt.numTraces());
            if (s.isSameDataA()) matmul.setSameDataMatrixA();
            if (s.isSameDataB()) matmul.setSameDataMatrixB();
            matmul.setGeneral(s.isGEMM());
            matmul.setPrecision(s.precisionA(),
                                s.precisionB(),
                                s.precisionC());
            matmul.setDimensions(s.heightC(), // M
                                 s.widthC(),  // N
                                 KfromA);     // K
            matmul.setDataLayout(s.isTransposeA(),
                                 s.isTransposeB());

            const vector< size_t > params = _jitMemo.autotuneLookup(matmul);

            if (params.empty())
            {
                _failure = true;
                return;
            }

            matmul.setParams(params);

            ArrayBuf* A
                = s.astvarA()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarA()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarA(), _vt);

            ArrayBuf* B
                = s.astvarB()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarB()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarB(), _vt);

            ArrayBuf* C
                = s.astvarC()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarC()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarC(), _vt);

            const double alpha = s.alpha();
            const double beta  = s.beta();

            const string kernelName = matmul.kernelName();
            stringstream ss;
            ss << matmul;
            const string kernelSource = ss.str();

#ifdef __LOGGING_ENABLED__
            ss.str(string());
            ss << kernelName
               << " A(var=";
            if (s.astvarA()->isTraceVariable())
                ss << s.astvarA()->variable();
            else
                ss << s.astvarA();
            ss << " W=" << A->width()
               << " H=" << A->height()
               << " prec=" << A->precision()
               << " vlen=" << A->vectorLength()
               << ") B(var=";
            if (s.astvarB()->isTraceVariable())
                ss << s.astvarB()->variable();
            else
                ss << s.astvarB();
            ss << " W=" << B->width()
               << " H=" << B->height()
               << " prec=" << B->precision()
               << " vlen=" << B->vectorLength()
               << ") C(var=";
            if (s.astvarC()->isTraceVariable())
                ss << s.astvarC()->variable();
            else
                ss << s.astvarC();
            ss << " W=" << C->width()
               << " H=" << C->height()
               << " prec=" << C->precision()
               << " vlen=" << C->vectorLength()
               << ")";
            LOGGER(ss.str())
#endif

            OCLkernel ckernel(_memMgr.getComputeDevice(),
                              kernelName,
                              kernelSource);

            if (! ckernel.isOk())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "compile error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            if (! matmul.setArgs(ckernel, A, B, C, alpha, beta))
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "set arguments error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            const vector< size_t > globalDims = matmul.globalWorkItems();
            const vector< size_t > localDims = matmul.localWorkItems();

            for (size_t i = 0; i < globalDims.size(); i++)
            {
                ckernel << OCLWorkIndex(globalDims[i], localDims[i]);

                if (! ckernel.statusOp())
                {
#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "set index space dimension error: " << i;
                    LOGGER(ss.str())
#endif
                    _failure = true;
                    return;
                }
            }

            _memMgr.getComputeDevice() << ckernel;

            if (! _memMgr.getComputeDevice().statusOp())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "enqueue error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            _memMgr.getComputeDevice() << FLUSH;

            if (! _memMgr.getComputeDevice().statusOp())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "wait error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            matmul.clearArgs();
        }
        else
        {
            // there are no other GPU matrix multiply implementations except
            // "Evergreen" (which works on both ATI and NVIDIA), so fail
            _failure = true;
        }
    }
    else if (s.isMATVEC())
    {
        // check if device supports the Evergreen matrix multiply
        if (OCLhacks::singleton().supportEvergreen(_memMgr.getDeviceIndex()))
        {
            // do not allow modulo stream array subscripting for N dimension
            const size_t NfromA = s.isTransposeA() ? s.heightA() : s.widthA();
            const size_t NfromB = s.widthB();
            if (NfromA != NfromB)
            {
                _failure = true;
                return;
            }

            // ATI Evergreen matrix-vector multiply
            Evergreen::MatmulMV matvec(_memMgr.getDeviceIndex());

            // exogenous parameters
            matvec.setPacking(_vt.numTraces());
            if (s.isSameDataA()) matvec.setSameDataMatrixA();
            matvec.setGeneral(s.isGEMM());
            matvec.setPrecision(s.precisionA(),
                                s.precisionB(),
                                s.precisionC());
            matvec.setDimensions(s.widthC(), // M
                                 NfromA);    // N
            matvec.setDataLayout(s.isTransposeA());

            const vector< size_t > params = _jitMemo.autotuneLookup(matvec);

            if (params.empty())
            {
                _failure = true;
                return;
            }

            matvec.setParams(params);

            ArrayBuf* A
                = s.astvarA()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarA()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarA(), _vt);

            ArrayBuf* B
                = s.astvarB()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarB()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarB(), _vt);

            ArrayBuf* C
                = s.astvarC()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarC()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarC(), _vt);

            const double alpha = s.alpha();
            const double beta  = s.beta();

            const string kernelName = matvec.kernelName();
            stringstream ss;
            ss << matvec;
            const string kernelSource = ss.str();

#ifdef __LOGGING_ENABLED__
            ss.str(string());
            ss << kernelName
               << " A(var=";
            if (s.astvarA()->isTraceVariable())
                ss << s.astvarA()->variable();
            else
                ss << s.astvarA();
            ss << " W=" << A->width()
               << " H=" << A->height()
               << " prec=" << A->precision()
               << " vlen=" << A->vectorLength()
               << ") B(var=";
            if (s.astvarB()->isTraceVariable())
                ss << s.astvarB()->variable();
            else
                ss << s.astvarB();
            ss << " W=" << B->width()
               << " H=" << B->height()
               << " prec=" << B->precision()
               << " vlen=" << B->vectorLength()
               << ") C(var=";
            if (s.astvarC()->isTraceVariable())
                ss << s.astvarC()->variable();
            else
                ss << s.astvarC();
            ss << " W=" << C->width()
               << " H=" << C->height()
               << " prec=" << C->precision()
               << " vlen=" << C->vectorLength()
               << ")";
            LOGGER(ss.str())
#endif

            OCLkernel ckernel(_memMgr.getComputeDevice(),
                              kernelName,
                              kernelSource);

            if (! ckernel.isOk())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "compile error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            if (! matvec.setArgs(ckernel, A, B, C, alpha, beta))
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "set arguments error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            const vector< size_t > globalDims = matvec.globalWorkItems();
            const vector< size_t > localDims = matvec.localWorkItems();

            for (size_t i = 0; i < globalDims.size(); i++)
            {
                ckernel << OCLWorkIndex(globalDims[i], localDims[i]);

                if (! ckernel.statusOp())
                {
#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "set index space dimension error: " << i;
                    LOGGER(ss.str())
#endif
                    _failure = true;
                    return;
                }
            }

            _memMgr.getComputeDevice() << ckernel;

            if (! _memMgr.getComputeDevice().statusOp())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "enqueue error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            _memMgr.getComputeDevice() << FLUSH;

            if (! _memMgr.getComputeDevice().statusOp())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "wait error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            matvec.clearArgs();
        }
        else
        {
            // there are no other GPU matrix multiply implementations except
            // "Evergreen" (which works on both ATI and NVIDIA), so fail
            _failure = true;
        }
    }
    else if (s.isVECMAT())
    {
        // check if device supports the Evergreen matrix multiply
        if (OCLhacks::singleton().supportEvergreen(_memMgr.getDeviceIndex()))
        {
            // do not allow modulo stream array subscripting for N dimension
            const size_t NfromA = s.widthA();
            const size_t NfromB = s.isTransposeB() ? s.widthB() : s.heightB();
            if (NfromA != NfromB)
            {
                _failure = true;
                return;
            }

            // ATI Evergreen matrix-vector multiply
            Evergreen::MatmulMV matvec(_memMgr.getDeviceIndex());

            // exogenous parameters
            matvec.setPacking(_vt.numTraces());
            if (s.isSameDataB()) matvec.setSameDataMatrixA();
            matvec.setGeneral(s.isGEMM());
            matvec.setPrecision(s.precisionB(),
                                s.precisionA(),
                                s.precisionC());
            matvec.setDimensions(s.widthC(), // M
                                 NfromB);    // N
            matvec.setDataLayout(! s.isTransposeB());

            const vector< size_t > params = _jitMemo.autotuneLookup(matvec);

            if (params.empty())
            {
                _failure = true;
                return;
            }

            matvec.setParams(params);

            ArrayBuf* A
                = s.astvarA()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarA()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarA(), _vt);

            ArrayBuf* B
                = s.astvarB()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarB()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarB(), _vt);

            ArrayBuf* C
                = s.astvarC()->isTraceVariable()
                      ? _memMgr.getArrayBuf(s.astvarC()->variable(), _vt)
                      : _memMgr.getArrayBuf(s.astvarC(), _vt);

            const double alpha = s.alpha();
            const double beta  = s.beta();

            const string kernelName = matvec.kernelName();
            stringstream ss;
            ss << matvec;
            const string kernelSource = ss.str();

#ifdef __LOGGING_ENABLED__
            ss.str(string());
            ss << kernelName
               << " A(var=";
            if (s.astvarA()->isTraceVariable())
                ss << s.astvarA()->variable();
            else
                ss << s.astvarA();
            ss << " W=" << A->width()
               << " H=" << A->height()
               << " prec=" << A->precision()
               << " vlen=" << A->vectorLength()
               << ") B(var=";
            if (s.astvarB()->isTraceVariable())
                ss << s.astvarB()->variable();
            else
                ss << s.astvarB();
            ss << " W=" << B->width()
               << " H=" << B->height()
               << " prec=" << B->precision()
               << " vlen=" << B->vectorLength()
               << ") C(var=";
            if (s.astvarC()->isTraceVariable())
                ss << s.astvarC()->variable();
            else
                ss << s.astvarC();
            ss << " W=" << C->width()
               << " H=" << C->height()
               << " prec=" << C->precision()
               << " vlen=" << C->vectorLength()
               << ")";
            LOGGER(ss.str())
#endif

            OCLkernel ckernel(_memMgr.getComputeDevice(),
                              kernelName,
                              kernelSource);

            if (! ckernel.isOk())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "compile error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            if (! matvec.setArgs(ckernel, B, A, C, alpha, beta))
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "set arguments error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            const vector< size_t > globalDims = matvec.globalWorkItems();
            const vector< size_t > localDims = matvec.localWorkItems();

            for (size_t i = 0; i < globalDims.size(); i++)
            {
                ckernel << OCLWorkIndex(globalDims[i], localDims[i]);

                if (! ckernel.statusOp())
                {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "set index space dimension error: " << i;
                LOGGER(ss.str())
#endif
                    _failure = true;
                    return;
                }
            }

            _memMgr.getComputeDevice() << ckernel;

            if (! _memMgr.getComputeDevice().statusOp())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "enqueue error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            _memMgr.getComputeDevice() << FLUSH;

            if (! _memMgr.getComputeDevice().statusOp())
            {
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "wait error: " << kernelName;
                LOGGER(ss.str())
#endif
                _failure = true;
                return;
            }

            matvec.clearArgs();
        }
        else
        {
            // there are no other GPU matrix multiply implementations except
            // "Evergreen" (which works on both ATI and NVIDIA), so fail
            _failure = true;
        }
    }
}

void XStmtEnqueue::visit(XStmtMatmulAuto& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // some compute devices are single precision only
    if (! OCLhacks::singleton().supportFP64(_memMgr.getDeviceIndex()) &&
        (sizeof(double) == s.precisionA() ||
         sizeof(double) == s.precisionB() ||
         sizeof(double) == s.precisionC()) )
    {
        _failure = true;
        return;
    }

    const size_t deviceIdx = _memMgr.getDeviceIndex();
    const double threshold = OCLhacks::singleton().paranoidError(deviceIdx);

    if (s.isMATMUL())
    {
        // check if device supports the Evergreen matrix multiply
        if (OCLhacks::singleton().supportEvergreen(_memMgr.getDeviceIndex()))
        {
            // do not allow modulo stream array subscripting for K dimension
            const size_t KfromA = s.isTransposeA() ? s.heightA() : s.widthA();
            const size_t KfromB = s.isTransposeB() ? s.widthB() : s.heightB();
            if (KfromA != KfromB)
            {
                _failure = true;
                return;
            }

            // ATI Evergreen matrix multiply
            Evergreen::MatmulMM matmul(_memMgr.getDeviceIndex());

            // exogenous parameters
            matmul.setPacking(_vt.numTraces());
            if (s.isSameDataA()) matmul.setSameDataMatrixA();
            if (s.isSameDataB()) matmul.setSameDataMatrixB();
            matmul.setGeneral(s.isGEMM());
            matmul.setPrecision(s.precisionA(),
                                s.precisionB(),
                                s.precisionC());
            matmul.setDimensions(s.heightC(), // M
                                 s.widthC(),  // N
                                 KfromA);     // K
            matmul.setDataLayout(s.isTransposeA(),
                                 s.isTransposeB());

            // check if have already autotuned this kernel (and perhaps failed)
            if (_jitMemo.autotuneSkip(matmul, _failure))
                return;

            bool foundGoodKernel = false, imagesDoNotWork = false;

            const size_t vlen0 = OCLhacks::singleton().supportImages(deviceIdx)
                                     ? 0  // use images
                                     : 1; // no image support

            // loop over vector lengths
            for (size_t vlenA = vlen0; vlenA <= 4; vlenA++)
            for (size_t vlenB = vlen0; vlenB <= 4; vlenB++)
            for (size_t vlenC = (s.isGEMM() ? 1 : vlen0); vlenC <= 4; vlenC++)
            {
                // 0 is image, 1|2|4 are memory buffers
                if (3 == vlenA || 3 == vlenB || 3 == vlenC)
                    continue; // skip

                // some devices do not support images
                if ( imagesDoNotWork &&
                     (0 == vlenA || 0 == vlenB || 0 == vlenC) )
                    continue; // skip

                // endogenous parameters
                matmul.activeOptimizeWorkGroup();
                matmul.activeOptimizeInnerBlocking();
                matmul.doNotOptimizeVectorLength();
                matmul.inactiveOptimizeExtraParam();
                matmul.setVectorLength(vlenA, vlenB, vlenC);
                matmul.setExtraParam(0);

                // find optimal kernel
                matmul.sentinelCheck();
                if (! _stdEM.searchLoop(matmul))
                {
                    // if the device does not support images, the OpenCL
                    // compiler does not always fail gracefully - sometimes
                    // there is a linkage error at runtime - so detecting
                    // lack of image support is good for stability as well
                    // as search speed
                    if (0 == vlenA || 0 == vlenB || 0 == vlenC)
                        imagesDoNotWork = true;
                    continue; // skip
                }

                // paranoid check
                matmul.paranoidCheck(threshold);
                if (-1 == _stdEM.timingLoop(matmul))
                {
#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "paranoid check failed: " << matmul.kernelName();
                    LOGGER(ss.str())
#endif
                    continue; // skip
                } 

                // timing benchmark
                matmul.noCheck();
                const double GFLOPS = _stdEM.timingLoop(matmul);
                if (-1 != GFLOPS)
                {
                    foundGoodKernel = true;
                    _jitMemo.autotuneGFLOPS(matmul, GFLOPS);
                }
                else
                {
#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "timing benchmark failed: " << matmul.kernelName();
                    LOGGER(ss.str())
#endif
                }
            }

            if (! foundGoodKernel)
            {
                _failure = true;
                _jitMemo.autotuneFail(matmul);
            }
            else
            {
                if (! _jitMemo.autotuneVars(matmul, s, *_xid))
                    _failure = true;
            }
        }
        else
        {
            _failure = true;
        }
    }
    else if (s.isMATVEC())
    {
        // check if device supports the Evergreen matrix multiply
        if (OCLhacks::singleton().supportEvergreen(_memMgr.getDeviceIndex()))
        {
            // do not allow modulo stream array subscripting for N dimension
            const size_t NfromA = s.isTransposeA() ? s.heightA() : s.widthA();
            const size_t NfromB = s.widthB();
            if (NfromA != NfromB)
            {
                _failure = true;
                return;
            }

            // ATI Evergreen matrix-vector multiply
            Evergreen::MatmulMV matvec(_memMgr.getDeviceIndex());

            // exogenous parameters
            matvec.setPacking(_vt.numTraces());
            if (s.isSameDataA()) matvec.setSameDataMatrixA();
            matvec.setGeneral(s.isGEMM());
            matvec.setPrecision(s.precisionA(),
                                s.precisionB(),
                                s.precisionC());
            matvec.setDimensions(s.widthC(),  // M
                                 NfromA);     // N
            matvec.setDataLayout(s.isTransposeA());

            // check if have already autotuned this kernel (and perhaps failed)
            if (_jitMemo.autotuneSkip(matvec, _failure))
                return;

            bool foundGoodKernel = false, imagesDoNotWork = false;

            const size_t vlen0 = OCLhacks::singleton().supportImages(deviceIdx)
                                     ? 0  // use images
                                     : 1; // no image support

#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "matvec vlen0: " << vlen0
                       << " device: " << deviceIdx;
                    LOGGER(ss.str())
#endif

            // loop over vector lengths
            for (size_t vlenA = vlen0; vlenA <= 4; vlenA++)
            for (size_t vlenB = vlen0; vlenB <= 4; vlenB++)
            for (size_t vlenC = (s.isGEMM() ? 1 : vlen0); vlenC <= 4; vlenC++)
            {
                // 0 is image, 1|2|4 are memory buffers
                if (3 == vlenA || 3 == vlenB || 3 == vlenC)
                    continue; // skip

                // some devices do not support images
                if ( imagesDoNotWork &&
                     (0 == vlenA || 0 == vlenB || 0 == vlenC) )
                    continue; // skip

                // endogenous parameters
                matvec.activeOptimizeWorkGroup();
                matvec.activeOptimizeInnerBlocking();
                matvec.doNotOptimizeVectorLength();
                matvec.inactiveOptimizeExtraParam();
                matvec.setVectorLength(vlenA, vlenB, vlenC);
                matvec.setExtraParam(0);

                // find optimal kernel
                matvec.sentinelCheck();
                if (! _stdEM.searchLoop(matvec))
                {
                    // if the device does not support images, the OpenCL
                    // compiler does not always fail gracefully - sometimes
                    // there is a linkage error at runtime - so detecting
                    // lack of image support is good for stability as well
                    // as search speed
                    if (0 == vlenA || 0 == vlenB || 0 == vlenC)
                        imagesDoNotWork = true;
                    continue; // skip
                }

                // paranoid check
                matvec.paranoidCheck(threshold);
                if (-1 == _stdEM.timingLoop(matvec))
                {
#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "paranoid check failed: " << matvec.kernelName();
                    LOGGER(ss.str())
#endif
                    continue; // skip
                } 

                // timing benchmark
                matvec.noCheck();
                const double GFLOPS = _stdEM.timingLoop(matvec);
                if (-1 != GFLOPS)
                {
                    foundGoodKernel = true;
                    _jitMemo.autotuneGFLOPS(matvec, GFLOPS);
                }
                else
                {
#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "timing benchmark failed: " << matvec.kernelName();
                    LOGGER(ss.str())
#endif
                }
            }

            if (! foundGoodKernel)
            {
                _failure = true;
                _jitMemo.autotuneFail(matvec);
            }
            else
            {
                if (! _jitMemo.autotuneVars(matvec, s, *_xid))
                    _failure = true;
            }
        }
        else
        {
            _failure = true;
        }
    }
    else if (s.isVECMAT())
    {
        // check if device supports the Evergreen matrix multiply
        if (OCLhacks::singleton().supportEvergreen(_memMgr.getDeviceIndex()))
        {
            // do not allow modulo stream array subscripting for N dimension
            const size_t NfromA = s.widthA();
            const size_t NfromB = s.isTransposeB() ? s.widthB() : s.heightB();
            if (NfromA != NfromB)
            {
                _failure = true;
                return;
            }

            // ATI Evergreen matrix-vector multiply
            Evergreen::MatmulMV matvec(_memMgr.getDeviceIndex());

            // exogenous parameters
            matvec.setPacking(_vt.numTraces());
            if (s.isSameDataB()) matvec.setSameDataMatrixA();
            matvec.setGeneral(s.isGEMM());
            matvec.setPrecision(s.precisionB(),
                                s.precisionA(),
                                s.precisionC());
            matvec.setDimensions(s.widthC(),  // M
                                 NfromB);     // N
            matvec.setDataLayout(! s.isTransposeB());

            // check if have already autotuned this kernel (and perhaps failed)
            if (_jitMemo.autotuneSkip(matvec, _failure))
                return;

            bool foundGoodKernel = false, imagesDoNotWork = false;

            const size_t vlen0 = OCLhacks::singleton().supportImages(deviceIdx)
                                     ? 0  // use images
                                     : 1; // no image support

            // loop over vector lengths
            for (size_t vlenA = vlen0; vlenA <= 4; vlenA++)
            for (size_t vlenB = vlen0; vlenB <= 4; vlenB++)
            for (size_t vlenC = (s.isGEMM() ? 1 : vlen0); vlenC <= 4; vlenC++)
            {
                // 0 is image, 1|2|4 are memory buffers
                if (3 == vlenA || 3 == vlenB || 3 == vlenC)
                    continue; // skip

                // some devices do not support images
                if ( imagesDoNotWork &&
                     (0 == vlenA || 0 == vlenB || 0 == vlenC) )
                    continue; // skip

                // endogenous parameters
                matvec.activeOptimizeWorkGroup();
                matvec.activeOptimizeInnerBlocking();
                matvec.doNotOptimizeVectorLength();
                matvec.inactiveOptimizeExtraParam();
                matvec.setVectorLength(vlenA, vlenB, vlenC);
                matvec.setExtraParam(0);

                // find optimal kernel
                matvec.sentinelCheck();
                if (! _stdEM.searchLoop(matvec))
                {
                    // if the device does not support images, the OpenCL
                    // compiler does not always fail gracefully - sometimes
                    // there is a linkage error at runtime - so detecting
                    // lack of image support is good for stability as well
                    // as search speed
                    if (0 == vlenA || 0 == vlenB || 0 == vlenC)
                        imagesDoNotWork = true;
                    continue; // skip
                }

                // paranoid check
                matvec.paranoidCheck(threshold);
                if (-1 == _stdEM.timingLoop(matvec))
                {
#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "paranoid check failed: " << matvec.kernelName();
                    LOGGER(ss.str())
#endif
                    continue; // skip
                } 

                // timing benchmark
                matvec.noCheck();
                const double GFLOPS = _stdEM.timingLoop(matvec);
                if (-1 != GFLOPS)
                {
                    foundGoodKernel = true;
                    _jitMemo.autotuneGFLOPS(matvec, GFLOPS);
                }
                else
                {
#ifdef __LOGGING_ENABLED__
                    stringstream ss;
                    ss << "timing benchmark failed: " << matvec.kernelName();
                    LOGGER(ss.str())
#endif
                }
            }

            if (! foundGoodKernel)
            {
                _failure = true;
                _jitMemo.autotuneFail(matvec);
            }
            else
            {
                if (! _jitMemo.autotuneVars(matvec, s, *_xid))
                    _failure = true;
            }
        }
        else
        {
            _failure = true;
        }
    }
}

void XStmtEnqueue::visit(XStmtReadData& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    s.readoutPtr()->backMem()->swizzle();

    // continuation: need same standard vector length hack as everywhere else
    const FrontMem* proto = s.readoutPtr()->frontMem().front();

    // standard vector length is double2 and float4
    const size_t standardVectorLength = proto->isDouble() ? 2 : 4;

    // special case for scalar reductions
    // relies on implicit convention of array dimensions
    const bool specialCaseReadScalar
        = 1 == proto->H() && standardVectorLength == proto->W();

    // if front memory is not even multiple of standard vector length
    const bool specialCasePadNeeded
        = 0 != (proto->W() % standardVectorLength);
    const size_t specialCasePadWidth
        = standardVectorLength * (1 + (proto->W() / standardVectorLength));

    // width is forced to be a multiple of standard vector length
    const size_t frontWidth = specialCasePadNeeded
                                  ? specialCasePadWidth
                                  : proto->W();

    // size of memory for each front memory box
    const size_t frontSize
        = proto->H() * frontWidth * (proto->isDouble() ? sizeof(double)
                                                       : sizeof(float));

    // continuation: front memory without backing memory
    for (size_t i = 0; i < s.readoutPtr()->frontMem().size(); i++)
    {
        if (NULL == s.readoutPtr()->frontMem()[i]->ptrMem())
        {
            s.readoutPtr()
                ->frontMem()[i]
                ->setBackMem(
                      static_cast< char* >(s.readoutPtr()->backMem()->ptrMem())
                          + i * frontSize,
                      s.readoutPtr()->backMem() );
        }
    }
}

void XStmtEnqueue::visit(XStmtReduce& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // vector length for private reduction variable
    size_t redVecLen = _minVectorLength;
    for (set< AstVariable* >::const_iterator
         it = s.rhsVariable().begin();
         it != s.rhsVariable().end();
         it++)
    {
        const size_t vecLen = getVectorLength(*it);
        if (vecLen > redVecLen)
            redVecLen = vecLen;
    }

    // get reduction variable, this is managed by the function object
    FloatPt* reduceVar = _func->getPrivateVar( s.lhsVariable()->precision(),
                                               redVecLen );

    // set accumulation variable to zero
    _func->assignZero(reduceVar);

    // RHS dimensions for the reduction loop
    size_t rhsWidth, rhsHeight;

    // accumulate over stream dimensions
    if (s.accumPtr())
    {
        rhsWidth = s.accumPtr()->argW();
        rhsHeight = s.accumPtr()->argH();
    }

    // dot product over bounding stream dimensions
    if (s.dotprodPtr())
    {
        rhsWidth = s.dotprodPtr()->leftW() > s.dotprodPtr()->rightW()
                       ? s.dotprodPtr()->leftW()
                       : s.dotprodPtr()->rightW();

        rhsHeight = s.dotprodPtr()->leftH() > s.dotprodPtr()->rightH()
                       ? s.dotprodPtr()->leftH()
                       : s.dotprodPtr()->rightH();
    }

    // begin outer loop down rows
    const bool useLoopH = 1 != rhsHeight;
    const size_t loopIndexH = useLoopH
                                  ? _func->loopBegin(
                                               0,
                                               rhsHeight )
                                  : -1;

    // begin inner loop across rows
    const size_t loopIndexW = _func->loopBegin(
                                         0,
                                         rhsWidth < redVecLen
                                             ? 1
                                             : rhsWidth / redVecLen );

    // new for-loop subscript
    ForLoopSubscript newSub( rhsWidth / redVecLen,
                             rhsHeight );

    newSub.setLoopVars(loopIndexW, loopIndexH);
    if (_subs.top()->isOuterLoopTiles())
    {
        newSub.setOuterLoopTiles( _subs.top()->getOuterLoopTileVar() );
    }
    if (_subs.top()->isGlobalIndexTiles())
    {
        newSub.setGlobalIndexTiles();
    }
    _subs.push(&newSub);

    // clear RHS statement string
    _rhsStmt.str(string());

    // disable the heuristic hack with dot() for scalars
    _scalarToScalar = true;

    // descend into body of loop here
    if (s.accumPtr()) s.accumPtr()->accept(*this);
    if (s.dotprodPtr()) s.dotprodPtr()->accept(*this);

    _scalarToScalar = false;

    // accumulation statement inside for-loop
    _func->incrementStatement(reduceVar, _rhsStmt.str());

    // end loops
    if (useLoopH) _func->loopEnd();
    _func->loopEnd();
    _subs.pop();

    // get LHS variable (kernel generation variable, not the AST variable)
    Variable* lhsVar = s.lhsVariable()->isTraceVariable()
                           ? _func->traceVar(s.lhsVariable()->variable())
                           : _func->splitVar(s.lhsVariable());

    // is the LHS variable in a read_scalar() statement?
    const bool isReadoutVar
        = ( s.lhsVariable()->isTraceVariable() &&
            0 == _xid->getReadoutDim(s.lhsVariable()->variable()) );

    // assigning accumulated scalar value so no array subscripts involved
    FixedSubscript newSubLHS(0);
    if (isReadoutVar) newSubLHS.setReadScalar();
    if (_subs.top()->isOuterLoopTiles())
    {
        newSubLHS.setOuterLoopTiles( _subs.top()->getOuterLoopTileVar() );
    }
    if (_subs.top()->isGlobalIndexTiles())
    {
        newSubLHS.setGlobalIndexTiles();
    }

    // special case if LHS variable is for read_scalar
    const size_t lhsVecLen = getVectorLength(s.lhsVariable());

    // accumulation is mean, need to take average
    const size_t divideBy = rhsWidth * rhsHeight;
    const bool isDivideBy = s.accumPtr() &&
                            s.accumPtr()->takeAvg() &&
                            divideBy > 1;

    // reduction variable identifier name
    _rhsStmt.str(string());
    reduceVar->identifierName(_rhsStmt);
    const string redVarName = _rhsStmt.str();

    if (_func->isPrivate(lhsVar))
    {
        // assign to a private variable (not a stream so no subscript)
        stringstream ss;
        ss << redVarName;
        if (isDivideBy) ss << "/ " << divideBy;
        _func->assignStatement(lhsVar, ss.str());
    }
    else // assign to a stream so need subscript
    {
        newSubLHS.setVariable( s.lhsVariable()->W(),
                               s.lhsVariable()->H(),
                               lhsVecLen );


        // the LHS and RHS variables may be different vector lengths
        if (lhsVecLen < redVecLen)
        {
            vector< string > reduceComponents;
            switch (lhsVecLen)
            {
                case (1) :
                    for (size_t i = 0; i < redVecLen; i++)
                    {
                        stringstream ss;
                        ss << ".s" << i;
                        reduceComponents.push_back(ss.str());
                    }
                    break;

                case (2) :
                    reduceComponents.push_back(".lo");
                    reduceComponents.push_back(".hi");
                    break;

                // case (4) is impossible as lhsVecLen < redVecLen
            }

            // must slice and accumulate the reduction variable to the LHS
            for (vector< string >::const_iterator
                 it = reduceComponents.begin();
                 it != reduceComponents.end();
                 it++)
            {
                stringstream ss;
                ss << redVarName << *it;
                if (isDivideBy) ss << " / " << divideBy;
                _func->incrementStatement(lhsVar, &newSubLHS, ss.str());
            }
        }
        else if (lhsVecLen > redVecLen)
        {
            // must slice the LHS to fit the smaller reduction variable
            newSubLHS.setMixedVectorLength(redVecLen);
            newSubLHS.setMixedComponentIdx(0);
            _func->assignStatement(lhsVar, &newSubLHS, redVarName);
        }
        else // lhsVecLen == redVecLen
        {
            // direct assignment is ok
            stringstream ss;
            ss << redVarName;
            if (isDivideBy) ss << " / " << divideBy;
            _func->assignStatement(lhsVar, &newSubLHS, ss.str());
        }
    }
}

void XStmtEnqueue::visit(XStmtRepeat& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // begin loop
    _func->loopBegin(0, s.numReps());

    // descend into body of loop
    s.stuffInside()->accept(*this);

    // end loop
    _func->loopEnd();
}

void XStmtEnqueue::visit(XStmtRNGrand& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    _failure = true;
}

void XStmtEnqueue::visit(XStmtRNGseed& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    _failure = true;
}

void XStmtEnqueue::visit(XStmtSendData& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    BackMem* backMem = NULL;

    if (s.arrayMemPtr())
    {
        // array memory comes from intermediate variable values
        backMem = s.arrayMemPtr()->backMem();
    }
    else if (s.makedataPtr())
    {
        // make data is new array data
        backMem = s.makedataPtr()->backMem();
    }

    if (! _jitMemo.createArraySendData(s.lhsVariable(), *_xid, backMem))
        _failure = true;
}

void XStmtEnqueue::visit(XStmtSingle& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // LHS variable must be a memory buffer. It must be writable. The only
    // way it could be an image is if it were also an argument to matmul().
    // Then is would also appear on the RHS. So it must also be readable.
    // As it must be readable and writable, it can not be an image. This
    // implies the vector length can never be zero.
    _lhsVectorLength = getVectorLength(s.lhsVariable());

    // RHS variable vector lengths
    _smallVectorLength = _largeVectorLength = _lhsVectorLength;
    for (set< AstVariable* >::const_iterator
         it = s.rhsVariable().begin();
         it != s.rhsVariable().end();
         it++)
    {
        size_t vecLen = getVectorLength(*it);
        if (0 == vecLen)
        {
            const size_t prec = _xid->getPrecision(*it);
            if (sizeof(double) == prec)
                vecLen = 2; // double2
            else
                vecLen = 4; // float4
        }
        if (vecLen < _smallVectorLength) _smallVectorLength = vecLen;
        if (vecLen > _largeVectorLength) _largeVectorLength = vecLen;
    }

    // stream dimensions come from the left hand side
    const size_t lhsWidth = s.lhsVariable()->W();
    const size_t lhsHeight = s.lhsVariable()->H();

    // if context is the global index space, then every statement with a
    // stream larger than one element needs a loop
    const bool everyStmtNeedsLoop
        = _subs.top()->isGlobalIndexTiles() &&
          lhsWidth * lhsHeight > _smallVectorLength;

    // if context does not match the statement stream dimensions
    const bool streamSizeMismatch
        = _subs.top()->boundingWidth() != lhsWidth / _smallVectorLength ||
          _subs.top()->boundingHeight() != lhsHeight;

    // check if a loop is necessary over the stream in the statement
    const bool needLoop = everyStmtNeedsLoop || streamSizeMismatch;

    // is a loop 1D or 2D?
    const bool useLoopH = 1 != lhsHeight;

    // new subscript for loop
    ForLoopSubscript newSub( lhsWidth / _smallVectorLength,
                             lhsHeight);

    // begin loop
    if (needLoop)
    {
        // for-loop index for height
        const size_t loopIdxH = useLoopH
                                    ? _func->loopBegin( 0, lhsHeight )
                                    : -1;

        // for-loop index for width
        const size_t loopIdxW
            = _func->loopBegin( 0, lhsWidth < _smallVectorLength
                                       ? 1
                                       : lhsWidth / _smallVectorLength );

        // for-loop subscript
        newSub.setLoopVars(loopIdxW, loopIdxH);
        newSub.setGlobalIndexTiles();

        _subs.push(&newSub);
    }

    // mixed variable vector lengths require multiple statements and components
    set< size_t > componentIdxSet;
    if (_smallVectorLength == _largeVectorLength)
    {
        // all vector lengths are the same, nothing special required
        componentIdxSet.insert(-1);
    }
    else
    {
        // mixed variable vector lengths in statement
        _subs.top()->setMixedVectorLength(_smallVectorLength);

        switch (_smallVectorLength)
        {
            case (1) :
                if (2 == _largeVectorLength)
                {
                    componentIdxSet.insert(0); // .s0 for vector length 2
                    componentIdxSet.insert(1); // .s1 for vector length 2
                }
                else
                {
                    componentIdxSet.insert(0); // .s0 for vector length 2, 4
                    componentIdxSet.insert(1); // .s1 for vector length 2, 4
                    componentIdxSet.insert(2); // .s2 for vector length 4
                    componentIdxSet.insert(3); // .s3 for vector length 4
                }
                break;

            case (2) :
                componentIdxSet.insert(0); // .lo for vector length 4
                componentIdxSet.insert(1); // .hi for vector length 4
                break;

            // can not be 4 as must be strictly less than large vector length
        }
    }

    // loop over components
    for (set< size_t >::const_iterator
         it = componentIdxSet.begin();
         it != componentIdxSet.end();
         it++)
    {
        // set variable component
        if (-1 != *it) _subs.top()->setMixedComponentIdx(*it);

        // clear RHS statement string
        _rhsStmt.str(string());

        _scalarToScalar = s.scalarToScalar();

        // descend into RHS
        s.lhsVariable()->getArg(0)->accept(*this); // 0th arg is the RHS

        _scalarToScalar = false;

        // get LHS variable
        Variable* lhsVar = s.lhsVariable()->isTraceVariable()
                               ? _func->traceVar(s.lhsVariable()->variable())
                               : _func->splitVar(s.lhsVariable());

        if (_func->isPrivate(lhsVar))
        {
            // add new assignment statement
            _func->assignStatement(lhsVar, NULL, _rhsStmt.str());
        }
        else
        {
            // LHS variable dimensions in subscript for index arithmetic
            _subs.top()->setVariable( lhsWidth,
                                      lhsHeight,
                                      _lhsVectorLength );

            // add new assignment statement
            _func->assignStatement(lhsVar, _subs.top(), _rhsStmt.str());
        }
    }

    _subs.top()->unsetMixedVectorLength();

    // end loop
    if (needLoop)
    {
        if (useLoopH) _func->loopEnd();
        _func->loopEnd();
        _subs.pop();
    }
}

}; // namespace chai_internal
