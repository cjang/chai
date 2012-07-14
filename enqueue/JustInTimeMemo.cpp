// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include "JustInTimeMemo.hpp"
#include "Logger.hpp"
#include "PrecType.hpp"

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

size_t JustInTimeMemo::lookupVecLength(const VarKey varKey,
                                       const size_t PREC)
{
    size_t vecLen;

    // choose a vector length for the variable
    if ( _varLength.count(varKey) && ! _varLength[varKey].empty() )
    {
        // optimal vector length from kernel timings
        vecLen = _varLength[varKey][0];

        // only distinguish between images and memory buffers
        if (0 == vecLen)
        {
            // leave image vector length alone
        }
        else
        {
            // use simple heuristic for memory buffer vector length
            vecLen = PrecType::vecLength(PREC);
        }
    }
    else
    {
        // nothing known, so use simple heuristic for vector length
        vecLen = PrecType::vecLength(PREC);
    }

    // check for backdoor forced vector length
    const uint32_t varNum = varKey.first;
    const bool forceVecLen = -1 != varNum && _forceVarLength.count(varNum);
    if (forceVecLen)
    {
        switch (_forceVarLength[varNum])
        {
            // must be specified vector length
            case (0) :
            case (1) :
            case (2) :
            case (4) :
                vecLen = _forceVarLength[varNum];
                break;

            // must be memory buffer, don't care what vector length
            case (7) :
                if (0 == vecLen)
                {
                    // change image to memory buffer
                    vecLen = PrecType::vecLength(PREC);
                }
                else
                {
                    // already memory buffer, don't do anything
                }
                break;
        }
    }

    return vecLen;
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

JustInTimeMemo::JustInTimeMemo(MemTrans& memMgr)
    : _memMgr(memMgr) { }

void JustInTimeMemo::clearTrace(void)
{
    _varLength.clear();
    _rejectParams.clear();
    _forceVarLength.clear();
}

size_t JustInTimeMemo::getVecLength(const uint32_t varNum,
                                    const size_t PREC)
{
    return lookupVecLength( VarKey(varNum, NULL),
                            PREC );
}

size_t JustInTimeMemo::getVecLength(const AstVariable* v)
{
    return lookupVecLength( astVarKey(v),
                            v->prec() );
}

void JustInTimeMemo::forceVecLength(const uint32_t varNum,
                                    const int vecLen)
{
    _forceVarLength[ varNum ] = vecLen;
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
                                  const StmtMatmulAuto& xstmt,
                                  const StmtIdSpace& xid)
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
             jt = (*it).second.begin(); jt != (*it).second.end(); jt++)
        {
            kernelGen.setParams(*jt);

            const size_t vecLenA = kernelGen.vecLengthA();
            const size_t vecLenB = kernelGen.vecLengthB();
            const size_t vecLenC = kernelGen.vecLengthC();

            if ( (readwriteA && 0 == vecLenA) ||
                 (readwriteB && 0 == vecLenB) ||
                 (readwriteC && 0 == vecLenC) )
            {
                // reject as incompatible memory type
                _rejectParams.insert(*jt);
            }
            else
            {
                // accept the combination of memory types
                foundGoodMem = true;

                _varLength[ keyA ].push_back( vecLenA );
                _varLength[ keyB ].push_back( vecLenB );
                _varLength[ keyC ].push_back( vecLenC );

#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << exoKey
                   << " A" << numA << ":" << vecLenA
                   << " B" << numB << ":" << vecLenB
                   << " C" << numC << ":" << vecLenC;
                LOGGER(ss.str())
#endif
            }
        }
    }

    return foundGoodMem;
}

void JustInTimeMemo::eligibleGather(const AstVariable* dataVariable,
                                    const StmtIdSpace& xid)
{
    uint32_t dataVarNum = -1;
    const AstVariable *dataVarPtr = NULL;

    const bool readwriteDataVariable =
        dataVariable->isTraceVariable()
            ? xid.isReadWrite( dataVarNum = dataVariable->variable() )
            : xid.isReadWrite( dataVarPtr = dataVariable );

    const VarKey keyData(dataVarNum, dataVarPtr);

    if (! readwriteDataVariable)
    {
        _varLength[ keyData ].push_back( 0 ); // images have vector length 0
    }
}

bool JustInTimeMemo::createArrayTemp(const AstVariable* v,
                                     StmtIdSpace& xid)
{
    if (v->isTraceVariable())
    {
        return _memMgr.createArrayTemp( v->variable(),
                                        convertMode(v->variable(), xid),
                                        v->prec(),
                                        getVecLength(v),
                                        v->W(),
                                        v->H(),
                                        v->slots(),
                                        xid.getVectorTrace() );
    }
    else
    {
        return _memMgr.createArrayTemp( v,
                                        convertMode(v, xid),
                                        v->prec(),
                                        getVecLength(v),
                                        v->W(),
                                        v->H(),
                                        v->slots(),
                                        xid.getVectorTrace() );
    }

    return false; // should never happen
}

bool JustInTimeMemo::createArraySendData(const AstVariable* v,
                                         StmtIdSpace& xid,
                                         BackMem* backMem)
{
    if (v->isTraceVariable())
    {
        return _memMgr.createArraySendData( v->variable(),
                                            convertMode(v->variable(), xid),
                                            getVecLength(v),
                                            xid.getVectorTrace(),
                                            backMem );
    }
    else
    {
        return _memMgr.createArraySendData( v,
                                            convertMode(v, xid),
                                            getVecLength(v),
                                            xid.getVectorTrace(),
                                            backMem );
    }

    return false; // should never happen
}

}; // namespace chai_internal
