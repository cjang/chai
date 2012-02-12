// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "JustInTimeMemo.hpp"
#include "Logger.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// memo and blackboard for the JIT

JustInTimeMemo::VarKey JustInTimeMemo::astVarKey(const AstVariable* v) const
{
    uint32_t varNum = -1;
    const AstVariable* varPtr = NULL;

    if (v->isTraceVariable())
        varNum = v->variable();
    else
        varPtr = v;

    return VarKey(varNum, varPtr);
}

size_t JustInTimeMemo::lookupVectorLength(const VarKey varKey,
                                          const bool isDouble)
{
    size_t vlen;

    // choose a vector length for the variable
    if ( _varLength.count(varKey) && ! _varLength[varKey].empty() )
    {
        // optimal vector length from kernel timings
        vlen = _varLength[varKey][0];

        // only distinguish between images and memory buffers
        if (0 == vlen)
        {
            return vlen;
        }
        else
        {
            // use simple heuristic for vector length
            if (isDouble)
            {
                vlen = 2; // double2
            }
            else
            {
                vlen = 4; // float4
            }
        }
    }
    else
    {
        // use simple heuristic for vector length
        if (isDouble)
        {
            vlen = 2; // double2
        }
        else
        {
            vlen = 4; // float4
        }
    }

    return vlen;
}

vector< size_t > JustInTimeMemo::autotuneLookup(const string& exoKey)
{
    if ( 0 == _badAutoKernels.count( exoKey ) &&
         0 != _goodAutoKernels.count( exoKey ) )
    {
        for (map< double, set< vector< size_t > > >::const_reverse_iterator
             it = _goodAutoKernels[ exoKey ].rbegin();
             it != _goodAutoKernels[ exoKey ].rend();
             it++)
        {
            for(set< vector< size_t > >::const_iterator
                jt = (*it).second.begin();
                jt != (*it).second.end();
                jt++)
            {
                if (0 == _rejectParams.count(*jt))
                    return *jt;
            }
        }
    }

    return vector< size_t >();
}

bool JustInTimeMemo::autotuneSkip(const string& exoKey,
                                  bool& isFailure) const
{
    if (_badAutoKernels.count( exoKey ))
        return isFailure = true;

    return _goodAutoKernels.count( exoKey );
}

void JustInTimeMemo::autotuneFail(const string& exoKey)
{
    _badAutoKernels.insert( exoKey );
    _goodAutoKernels.erase( exoKey );

    LOGGER(exoKey)
}

JustInTimeMemo::JustInTimeMemo(MemManager& memMgr)
    : _memMgr(memMgr) { }

void JustInTimeMemo::clearTrace(void)
{
    _varLength.clear();
    _rejectParams.clear();
}

size_t JustInTimeMemo::getVectorLength(const uint32_t varNum,
                                       const size_t precision)
{
    return lookupVectorLength( VarKey(varNum, NULL),
                               sizeof(double) == precision );
}

size_t JustInTimeMemo::getVectorLength(const AstVariable* v)
{
    return lookupVectorLength( astVarKey(v),
                               v->isDouble() );
}

vector< size_t > JustInTimeMemo::autotuneLookup(
                                     const Evergreen::MatmulMMBase& kernelGen)
{
    return autotuneLookup( kernelGen.exogenousKey() );
}

vector< size_t > JustInTimeMemo::autotuneLookup(
                                     const Evergreen::MatmulMVBase& kernelGen)
{
    return autotuneLookup( kernelGen.exogenousKey() );
}

bool JustInTimeMemo::autotuneSkip(const Evergreen::MatmulMMBase& kernelGen,
                                  bool& isFailure) const
{
    return autotuneSkip( kernelGen.exogenousKey(), isFailure );
}

bool JustInTimeMemo::autotuneSkip(const Evergreen::MatmulMVBase& kernelGen,
                                  bool& isFailure) const
{
    return autotuneSkip( kernelGen.exogenousKey(), isFailure );
}

bool JustInTimeMemo::autotuneGFLOPS(const Evergreen::MatmulMMBase& kernelGen,
                                    const double GFLOPS)
{
    const string exoKey = kernelGen.exogenousKey();

    bool recordOk;

    vector< size_t > params;

    if (recordOk = kernelGen.getParams( params ))
    {
        _goodAutoKernels[ exoKey ]
                        [ GFLOPS ].insert( params );

#ifdef __LOGGING_ENABLED__
        stringstream ss;
        ss << exoKey << " GFLOPS " << GFLOPS;
        LOGGER(ss.str())
#endif
    }

    return recordOk;
}

bool JustInTimeMemo::autotuneGFLOPS(const Evergreen::MatmulMVBase& kernelGen,
                                    const double GFLOPS)
{
    const string exoKey = kernelGen.exogenousKey();

    bool recordOk;

    vector< size_t > params;

    if (recordOk = kernelGen.getParams( params ))
    {
        _goodAutoKernels[ exoKey ]
                        [ GFLOPS ].insert( params );

#ifdef __LOGGING_ENABLED__
        stringstream ss;
        ss << exoKey << " GFLOPS " << GFLOPS;
        LOGGER(ss.str())
#endif
    }

    return recordOk;
}

void JustInTimeMemo::autotuneFail(const Evergreen::MatmulMMBase& kernelGen)
{
    autotuneFail( kernelGen.exogenousKey() );
}

void JustInTimeMemo::autotuneFail(const Evergreen::MatmulMVBase& kernelGen)
{
    autotuneFail( kernelGen.exogenousKey() );
}

bool JustInTimeMemo::autotuneVars(Evergreen::MatmulBase& kernelGen,
                                  const XStmtMatmulAuto& xstmt,
                                  const XStmtIdSpace& xid)
{
    const string exoKey = kernelGen.exogenousKey();

    // if all kernels are bad, then no variable to memory assignments either
    if (_badAutoKernels.count( exoKey ))
        return false;

    // memory buffers can be: read-only, write-only, read-write
    // images can be: read-only, write-only

    // Memory buffers are always ok for any variable. Images are only
    // compatible with variables that are read-only or write-only. If
    // the variable is read-write, then images (vector length 0) are
    // disallowed.

    uint32_t numA = -1, numB = -1, numC = -1;
    const AstVariable *ptrA = NULL, *ptrB = NULL, *ptrC = NULL;

    // argument for matrix A
    const bool readwriteA =
        xstmt.astvarA()->isTraceVariable()
            ? xid.isReadWrite( numA = xstmt.astvarA()->variable() )
            : xid.isReadWrite( ptrA = xstmt.astvarA() );

    // argument for matrix B
    const bool readwriteB =
        xstmt.astvarB()->isTraceVariable()
            ? xid.isReadWrite( numB = xstmt.astvarB()->variable() )
            : xid.isReadWrite( ptrB = xstmt.astvarB() );

    // argument for matrix C
    const bool readwriteC =
        xstmt.astvarC()->isTraceVariable()
            ? xid.isReadWrite( numC = xstmt.astvarC()->variable() )
            : xid.isReadWrite( ptrC = xstmt.astvarC() );

    const VarKey keyA(numA, ptrA);
    const VarKey keyB(numB, ptrB);
    const VarKey keyC(numC, ptrC);

    bool foundGoodMem = false;

    // descend from fastest GFLOPS kernels to slowest
    for (map< double, set< vector< size_t > > >::const_reverse_iterator
         it = _goodAutoKernels[ exoKey ].rbegin();
         it != _goodAutoKernels[ exoKey ].rend();
         it++)
    {
        // iterate over all kernel generation parameters
        for (set< vector< size_t > >::const_iterator
             jt = (*it).second.begin();
             jt != (*it).second.end();
             jt++)
        {
            kernelGen.setParams(*jt);

            const size_t vlenA = kernelGen.vectorLengthA();
            const size_t vlenB = kernelGen.vectorLengthB();
            const size_t vlenC = kernelGen.vectorLengthC();

            if ( (readwriteA && 0 == vlenA) ||
                 (readwriteB && 0 == vlenB) ||
                 (readwriteC && 0 == vlenC) )
            {
                // reject as incompatible memory type
                _rejectParams.insert(*jt);
            }
            else
            {
                // accept the combination of memory types
                foundGoodMem = true;

                _varLength[ keyA ].push_back( vlenA );
                _varLength[ keyB ].push_back( vlenB );
                _varLength[ keyC ].push_back( vlenC );

#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << exoKey
                   << " A" << numA << ":" << vlenA
                   << " B" << numB << ":" << vlenB
                   << " C" << numC << ":" << vlenC;
                LOGGER(ss.str())
#endif
            }
        }
    }

    return foundGoodMem;
}

bool JustInTimeMemo::createArrayTemp(const AstVariable* v,
                                     XStmtIdSpace& xid)
{
    const size_t vectorLength = getVectorLength(v);
    const size_t packing      = v->isSameDataAcrossTraces()
                                    ? 1
                                    : xid.numTraces();

    if (v->isTraceVariable())
    {
        const uint32_t varNum = v->variable();

        if (xid.isReadOnly(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::READ,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            v->precision(),
                                            vectorLength,
                                            xid.getVectorTrace() );
        }
        else if (xid.isWriteOnly(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::WRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            v->precision(),
                                            vectorLength,
                                            xid.getVectorTrace() );
        }
        else if (xid.isReadWrite(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::READWRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            v->precision(),
                                            vectorLength,
                                            xid.getVectorTrace() );
        }
    }
    else
    {
        const AstVariable* varPtr = v;

        if (xid.isReadOnly(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::READ,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            v->precision(),
                                            vectorLength,
                                            xid.getVectorTrace() );
        }
        else if (xid.isWriteOnly(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::WRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            v->precision(),
                                            vectorLength,
                                            xid.getVectorTrace() );
        }
        else if (xid.isReadWrite(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::READWRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            v->precision(),
                                            vectorLength,
                                            xid.getVectorTrace() );
        }
    }

    return false; // should never happen
}

bool JustInTimeMemo::createArrayTemp(const AstVariable* v,
                                     XStmtIdSpace& xid,
                                     const float val32)
{
    const size_t vectorLength = getVectorLength(v);
    const size_t packing      = v->isSameDataAcrossTraces()
                                    ? 1
                                    : xid.numTraces();

    if (v->isTraceVariable())
    {
        const uint32_t varNum = v->variable();

        if (xid.isReadOnly(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::READ,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val32 );
        }
        else if (xid.isWriteOnly(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::WRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val32 );
        }
        else if (xid.isReadWrite(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::READWRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val32 );
        }
    }
    else
    {
        const AstVariable* varPtr = v;

        if (xid.isReadOnly(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::READ,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val32 );
        }
        else if (xid.isWriteOnly(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::WRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val32 );
        }
        else if (xid.isReadWrite(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::READWRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val32 );
        }
    }

    return false; // should never happen
}

bool JustInTimeMemo::createArrayTemp(const AstVariable* v,
                                     XStmtIdSpace& xid,
                                     const double val64)
{
    const size_t vectorLength = getVectorLength(v);
    const size_t packing      = v->isSameDataAcrossTraces()
                                    ? 1
                                    : xid.numTraces();

    if (v->isTraceVariable())
    {
        const uint32_t varNum = v->variable();

        if (xid.isReadOnly(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::READ,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val64 );
        }
        else if (xid.isWriteOnly(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::WRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val64 );
        }
        else if (xid.isReadWrite(varNum))
        {
            return _memMgr.createArrayTemp( varNum,
                                            MemManager::READWRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val64 );
        }
    }
    else
    {
        const AstVariable* varPtr = v;

        if (xid.isReadOnly(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::READ,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val64 );
        }
        else if (xid.isWriteOnly(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::WRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val64 );
        }
        else if (xid.isReadWrite(varPtr))
        {
            return _memMgr.createArrayTemp( varPtr,
                                            MemManager::READWRITE,
                                            packing,
                                            v->W(),
                                            v->H(),
                                            vectorLength,
                                            xid.getVectorTrace(),
                                            val64 );
        }
    }

    return false; // should never happen
}

bool JustInTimeMemo::createArraySendData(const AstVariable* v,
                                         XStmtIdSpace& xid,
                                         BackMem* backMem)
{
    const size_t vectorLength = getVectorLength(v);

    if (v->isTraceVariable())
    {
        const uint32_t varNum = v->variable();

        if (xid.isReadOnly(varNum))
        {
            return _memMgr.createArraySendData( varNum,
                                                MemManager::READ,
                                                vectorLength,
                                                xid.getVectorTrace(),
                                                backMem );
        }
        else if (xid.isWriteOnly(varNum))
        {
            return _memMgr.createArraySendData( varNum,
                                                MemManager::WRITE,
                                                vectorLength,
                                                xid.getVectorTrace(),
                                                backMem );
        }
        else if (xid.isReadWrite(varNum))
        {
            return _memMgr.createArraySendData( varNum,
                                                MemManager::READWRITE,
                                                vectorLength,
                                                xid.getVectorTrace(),
                                                backMem );
        }
    }
    else
    {
        const AstVariable* varPtr = v;

        if (xid.isReadOnly(varPtr))
        {
            return _memMgr.createArraySendData( varPtr,
                                                MemManager::READ,
                                                vectorLength,
                                                xid.getVectorTrace(),
                                                backMem );
        }
        else if (xid.isWriteOnly(varPtr))
        {
            return _memMgr.createArraySendData( varPtr,
                                                MemManager::WRITE,
                                                vectorLength,
                                                xid.getVectorTrace(),
                                                backMem );
        }
        else if (xid.isReadWrite(varPtr))
        {
            return _memMgr.createArraySendData( varPtr,
                                                MemManager::READWRITE,
                                                vectorLength,
                                                xid.getVectorTrace(),
                                                backMem );
        }
    }

    return false; // should never happen
}

}; // namespace chai_internal
