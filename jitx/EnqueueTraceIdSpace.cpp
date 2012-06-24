// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>
#include <vector>

#include "AstVariable.hpp"
#include "EnqueueTrace.hpp"
#include "OCLhacks.hpp"
#include "StmtIdSpace.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtIdSpace& s)
{
    if (_failure) return;

    // set containing context of ID space
    _xid = &s;

    // stream dimensions distinguish between code and data movement
    if (0 == s.streamW() && 0 == s.streamH())
    {
        if (StmtIdSpace::SPECIAL_OPENCL == s.specialK())
        {
            // inline OpenCL

            for (vector< Stmt* >::const_iterator
                 it = s.stuffInside().begin();
                 it != s.stuffInside().end();
                 it++)
            {
                (*it)->accept(*this);
            }
        }
        else
        {
            // no code, only data movement

            // constrain vector length choices by JIT
            for (map< uint32_t, int >::const_iterator
                 it = _vt.forceVectorLength().begin();
                 it != _vt.forceVectorLength().end();
                 it++)
            {
                _jitMemo.forceVectorLength( (*it).first,
                                            (*it).second );
            }

            for (vector< Stmt* >::const_iterator
                 it = s.stuffInside().begin();
                 it != s.stuffInside().end();
                 it++)
            {
                (*it)->accept(*this);
            }
        }
    }
    else
    {
        // must be code, need to build and run kernels

        if (StmtIdSpace::NOTHING_SPECIAL == s.specialK())
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
                            new AddrMem(prec, vlen, POINTER, GLOBAL) );
                }
                else if (s.isReadOnly(varNum))
                {
                    if (0 == vlen)
                        _func->addArgument(
                            varNum,
                            new Image2D(prec, vlen, READONLY) );
                    else
                        _func->addArgument(
                            varNum,
                            new AddrMem(prec, vlen, CONST_POINTER, GLOBAL) );
                }
                else if (s.isWriteOnly(varNum))
                {
                    if (0 == vlen)
                        _func->addArgument(
                            varNum,
                            new Image2D(prec, vlen, WRITEONLY) );
                    else
                        _func->addArgument(
                            varNum,
                            new AddrMem(prec, vlen, POINTER, GLOBAL) );
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
                            new AddrMem(prec, vlen, POINTER, GLOBAL) );
                }
                else if (s.isReadOnly(varPtr))
                {
                    if (0 == vlen)
                        _func->addArgument(
                            varPtr,
                            new Image2D(prec, vlen, READONLY) );
                    else
                        _func->addArgument(
                            varPtr,
                            new AddrMem(prec, vlen, CONST_POINTER, GLOBAL) );
                }
                else if (s.isWriteOnly(varPtr))
                {
                    if (0 == vlen)
                        _func->addArgument(
                            varPtr,
                            new Image2D(prec, vlen, WRITEONLY) );
                    else
                        _func->addArgument(
                            varPtr,
                            new AddrMem(prec, vlen, POINTER, GLOBAL) );
                }
            }

            // min and max variable effective vector lengths (images = 2|4)
            boundVectorLength();

            const size_t padW
                = (0 != (s.streamW() % _minVectorLength))
                  ? _minVectorLength * (1 + (s.streamW() / _minVectorLength))
                  : s.streamW();

            // data blocking between index space and outer loop
            Subscript* newSub;
            const bool scalarReduction = 1 == s.streamW() && 1 == s.streamH();
            const bool globalIndexOverThreads
                = s.numTraces() > ( scalarReduction ? 1 : padW * s.streamH() );

            if (globalIndexOverThreads)
            {
                // global index space is 1D over thread traces
                // no outer loop but stream assignments have 2D for-loops

                if ( scalarReduction ||
                     padW * s.streamH() == _minVectorLength )

                {
                    // stream size is a single vector element
                    newSub = new FixedSubscript(0);
                }
                else
                {
                    // stream has width and height
                    newSub = new ForLoopSubscript( padW, s.streamH() );
                }

                newSub->setGlobalIndexTiles();
            }
            else
            {
                // global index space is 2D over the array stream
                // outer loop is over thread traces

                // check for case of kernel with 1x1 scalar reduction
                if ( scalarReduction ||
                     padW * s.streamH() == _minVectorLength )
                {
                    newSub = new FixedSubscript(0);
                }
                else
                {
                    // assume width is at least the minimum vector length
                    newSub = new StreamSubscript(
                                     padW / _minVectorLength,
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

            // (UseRegister) initialize registers for trace memory variables
            for (set< uint32_t >::const_iterator
                 it = _xid->traceUseRegister().begin();
                 it != _xid->traceUseRegister().end();
                 it++)
            {
                const uint32_t varNum = *it;

                AddrMem* regVar = _func->createPrivate(
                                      varNum,
                                      _xid->getPrecision(varNum),
                                      getVectorLength(varNum) );
            }

            // body of function
            for (vector< Stmt* >::const_iterator
                 it = s.stuffInside().begin();
                 it != s.stuffInside().end();
                 it++)
            {
                (*it)->accept(*this);
            }

            // same data across traces suppresses data tile part of subscript
            set< uint32_t > traceSuppressTile = _func->gatherTraceVar();
            set< uint32_t > suppressTile;
            for (set< uint32_t >::const_iterator
                 it = traceSuppressTile.begin();
                 it != traceSuppressTile.end();
                 it++)
            {
                const uint32_t varNum = *it;

                const vector< size_t >
                    frontIndex = _vt.memallocFrontIndex( varNum );

                set< size_t > frontSet;

                for (vector< size_t >::const_iterator
                     it = frontIndex.begin();
                     it != frontIndex.end();
                     it++)
                {
                    frontSet.insert(*it);
                }

                if (1 == frontSet.size())
                    suppressTile.insert(varNum);
            }

            // gather variable declarations
            _func->gatherDecl(*_subs.top(), suppressTile);

            // (UseRegister) write registers back to trace memory variables
            for (set< uint32_t >::const_iterator
                 it = _xid->traceUseRegisterWriteBack().begin();
                 it != _xid->traceUseRegisterWriteBack().end();
                 it++)
            {
                const uint32_t varNum = *it;

                Variable* regVar = _func->traceVarPrivate(varNum);

                Variable* memVar = _func->traceVar(varNum);

                _func->assignVariables(memVar, _subs.top(), regVar);
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

                    const size_t effwidth = scalarReduction
                                                ? s.streamW()
                                                : padW / _minVectorLength;

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
        else if (StmtIdSpace::SPECIAL_MATMUL == s.specialK())
        {
            // body of function
            for (vector< Stmt* >::const_iterator
                 it = s.stuffInside().begin();
                 it != s.stuffInside().end();
                 it++)
            {
                (*it)->accept(*this);
            }
        }
        else if (StmtIdSpace::SPECIAL_MATMUL_AUTO == s.specialK())
        {
            // body of function
            for (vector< Stmt* >::const_iterator
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

}; // namespace chai_internal
