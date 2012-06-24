// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "EnqueueTrace.hpp"
#include "EvergreenMatmulBase.hpp"
#include "EvergreenMatmulMM.hpp"
#include "EvergreenMatmulMV.hpp"
#include "Logger.hpp"
#include "OCLhacks.hpp"
#include "PrecType.hpp"
#include "StmtMatmulAuto.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitStmt

void EnqueueTrace::visit(StmtMatmulAuto& s)
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

}; // namespace chai_internal
