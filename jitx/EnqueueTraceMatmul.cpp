// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>
#include <vector>

#include "EnqueueTrace.hpp"
#include "EvergreenMatmulBase.hpp"
#include "EvergreenMatmulMM.hpp"
#include "EvergreenMatmulMV.hpp"
#include "Logger.hpp"
#include "OCLdevice.hpp"
#include "OCLhacks.hpp"
#include "PrecType.hpp"
#include "StmtMatmul.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtMatmul& s)
{
    if (_failure) return;
    if (_printer) _printer->visit(s);

    // some compute devices are single precision only
    if (! OCLhacks::singleton().supportFP64(_memMgr.getDeviceIndex()) &&
        (PrecType::Double == s.precisionA() ||
         PrecType::Double == s.precisionB() ||
         PrecType::Double == s.precisionC()) )
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

            OCLkernel ckernel(_memMgr.getComputeDevice());
            ckernel.buildJIT(kernelName,
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

            OCLkernel ckernel(_memMgr.getComputeDevice());
            ckernel.buildJIT(kernelName,
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

            OCLkernel ckernel(_memMgr.getComputeDevice());
            ckernel.buildJIT(kernelName,
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

}; // namespace chai_internal
