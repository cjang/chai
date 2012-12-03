// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <cmath>
#include <map>
#include <sstream>

#include "CodeControlFlow.hpp"
#include "CodeFunction.hpp"
#include "CodeImageSample.hpp"
#include "CodeStatements.hpp"
#include "CodeType.hpp"
#include "CodeValues.hpp"
#include "KernelGeneratorMatmul.hpp"
#include "Logger.hpp"
#include "PrecType.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

using namespace AutoParamsMatmul;

////////////////////////////////////////
// GEMM kernel generator

bool KernelGeneratorMatmul::setArgsInternal(OCLkernel& ckernel,
                                            ArrayBuf* A,
                                            ArrayBuf* B,
                                            ArrayBuf* C,
                                            const double alpha,
                                            const double beta)
{
    if (NULL != A)
    {
        delete _A;
        _A = A;
    }

    if (NULL != B)
    {
        delete _B;
        _B = B;
    }

    if (NULL != C)
    {
        delete _C;
        _C = C;
    }

    _alpha = alpha;
    _beta = beta;

    // set kernel arguments for A, B, C arrays
    if (! _C->argumentToKernel(ckernel)) return false;
    if (! _A->argumentToKernel(ckernel)) return false;
    if (! _B->argumentToKernel(ckernel)) return false;

    // set local memory for array A
    if (_A->isMembuf())
    {
        const size_t numberElems = localSize() *
                                   groupSize() *
                                   blockW() *
                                   blockH();

        switch (_A->prec())
        {
            case (PrecType::UInt32) :
                {
                OCLlocalbuf<uint32_t> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Int32) :
                {
                OCLlocalbuf<int32_t> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Float) :
                {
                OCLlocalbuf<float> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Double) :
                {
                OCLlocalbuf<double> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;
        }
    }

    // set local memory for array B
    if (_B->isMembuf())
    {
        const size_t numberElems = localSize() *
                                   groupSize() *
                                   blockW() *
                                   blockW();

        switch (_B->prec())
        {
            case (PrecType::UInt32) :
                {
                OCLlocalbuf<uint32_t> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Int32) :
                {
                OCLlocalbuf<int32_t> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Float) :
                {
                OCLlocalbuf<float> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Double) :
                {
                OCLlocalbuf<double> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;
        }
    }

    // array dimensions
    if (! inlineDim())
    {
        if (! (ckernel << static_cast<int>(M())).statusOp())
            return false;

        if (! (ckernel << static_cast<int>(N())).statusOp())
            return false;

        if (! (ckernel << static_cast<int>(K())).statusOp())
            return false;
    }

    // GEMM alpha and beta
    if (general())
    {
        // alpha
        switch ( max<size_t>(_A->prec(), _B->prec()) )
        {
            case (PrecType::UInt32) :
                {
                    const uint32_t alphaK = alpha;
                    if (! (ckernel << alphaK).statusOp())
                        return false;
                }
                break;

            case (PrecType::Int32) :
                {
                    const int32_t alphaK = alpha;
                    if (! (ckernel << alphaK).statusOp())
                        return false;
                }
                break;

            case (PrecType::Float) :
                {
                    const float alphaK = alpha;
                    if (! (ckernel << alphaK).statusOp())
                        return false;
                }
                break;

            case (PrecType::Double) :
                {
                    const double alphaK = alpha;
                    if (! (ckernel << alphaK).statusOp())
                        return false;
                }
                break;
        }

        // beta
        switch (_C->prec())
        {
            case (PrecType::UInt32) :
                {
                    const uint32_t betaK = beta;
                    if (! (ckernel << betaK).statusOp())
                        return false;
                }
                break;

            case (PrecType::Int32) :
                {
                    const int32_t betaK = beta;
                    if (! (ckernel << betaK).statusOp())
                        return false;
                }
                break;

            case (PrecType::Float) :
                {
                    const float betaK = beta;
                    if (! (ckernel << betaK).statusOp())
                        return false;
                }
                break;

            case (PrecType::Double) :
                {
                    const double betaK = beta;
                    if (! (ckernel << betaK).statusOp())
                        return false;
                }
                break;
        }
    }

    return true;
}

KernelGeneratorMatmul::KernelGeneratorMatmul(const size_t deviceIndex)
    : ParamInlineDim(getExtraParam()),
      ParamLoopOrder(getExtraParam()),
      ParamGlobalID(getExtraParam()),
      _A(NULL),
      _B(NULL),
      _C(NULL),
      _checkOutputMode(NO_CHECK),
      _checkOutputTouched(false),
      _paranoidLimit(0),
      _alpha(1),
      _beta(1),
      _deviceIndex(deviceIndex) { }

KernelGeneratorMatmul::~KernelGeneratorMatmul(void)
{
    delete _A;
    delete _B;
    delete _C;
}

void KernelGeneratorMatmul::noCheck(void)
{
    _checkOutputMode = NO_CHECK;
    _checkOutputTouched = true;
}

void KernelGeneratorMatmul::sentinelCheck(void)
{
    _checkOutputMode = SENTINEL_CHECK;
    _checkOutputTouched = true;
}

void KernelGeneratorMatmul::paranoidCheck(const double absDiffLimit)
{
    _checkOutputMode = PARANOID_CHECK;
    _checkOutputTouched = true;
    _paranoidLimit   = absDiffLimit;
}

bool KernelGeneratorMatmul::setArgs(OCLdevice& cdev,
                                    OCLkernel& ckernel)
{
    cdev.scavenge();

    ArrayBuf *A = NULL, *B = NULL, *C = NULL;

    // allocate arrays
    if (NULL == _A || NULL == _B || NULL == _C ||
        _checkOutputTouched ||
        batchingChanged() ||
        generalChanged() ||
        precisionChanged() ||
        dimensionsChanged() ||
        layoutChanged() ||
        vectorLengthChanged())
    {
        _checkOutputTouched = true;

        A = transposeA()
                ? new ArrayBuf(cdev,
                               ArrayBuf::READ,
                               precA(),
                               vecLengthA(),
                               M(),
                               K(),
                               getSameDataMatrixA() ? 1 : batching())
                : new ArrayBuf(cdev,
                               ArrayBuf::READ,
                               precA(),
                               vecLengthA(),
                               K(),
                               M(),
                               getSameDataMatrixA() ? 1 : batching());

        B = transposeB()
                ? new ArrayBuf(cdev,
                               ArrayBuf::READ,
                               precB(),
                               vecLengthB(),
                               K(),
                               N(),
                               getSameDataMatrixB() ? 1 : batching())
                : new ArrayBuf(cdev,
                               ArrayBuf::READ,
                               precB(),
                               vecLengthB(),
                               N(),
                               K(),
                               getSameDataMatrixB() ? 1 : batching());

        C = new ArrayBuf(cdev,
                         general() ? ArrayBuf::READWRITE : ArrayBuf::WRITE,
                         precC(),
                         vecLengthC(),
                         N(),
                         M(),
                         batching(),
                         true); // host array memory for checking output

        if (! A->bufOk() || ! B->bufOk() || ! C->bufOk())
        {
            delete A;
            delete B;
            delete C;

            return false;
        }

        // initialize input arrays
        switch (_checkOutputMode)
        {
            case (SENTINEL_CHECK) :
                A->fillOnes();
                B->fillOnes();
                break;

            case (PARANOID_CHECK) :
                A->fillRandom();
                B->fillRandom();
                break;
        }
    }

    // initialize output array
    if (C)
    {
        // allocated a new output array
        switch (_checkOutputMode)
        {
            case (SENTINEL_CHECK) :
                if (general())
                    C->fillOnes();
                else
                    C->fillZeros();
                break;

            case (PARANOID_CHECK) :
                C->fillRandom();
                break;
        }
    }
    else
    {
        // using the old output array
        switch (_checkOutputMode)
        {
            case (SENTINEL_CHECK) :
                if (general())
                    _C->fillOnes();
                else
                    _C->fillZeros();
                break;

            case (PARANOID_CHECK) :
                _C->fillRandom();
                break;
        }
    }

    double alpha;
    if ( PrecType::isTypeFP(precA()) &&
         PrecType::isTypeFP(precB()) )
    {
        if (PARANOID_CHECK == _checkOutputMode)
            alpha = posrand<double>();
        else
            alpha = 1;
    }
    else
    {
        alpha = 1;
    }

    double beta;
    if ( PrecType::isTypeFP(precC()) )
    {
        if (PARANOID_CHECK == _checkOutputMode)
            beta = posrand<double>();
        else
            beta = 1;
    }
    else
    {
        beta = 1;
    }

    return setArgsInternal(ckernel, A, B, C, alpha, beta);
}

bool KernelGeneratorMatmul::setArgs(OCLkernel& ckernel,
                                    ArrayBuf* A,
                                    ArrayBuf* B,
                                    ArrayBuf* C,
                                    const double alpha,
                                    const double beta)
{
    return setArgsInternal(ckernel, A, B, C, alpha, beta);
}

void KernelGeneratorMatmul::clearArgs(void)
{
    // do not delete array buffer objects, they are owned by the caller
    _A = NULL;
    _B = NULL;
    _C = NULL;

    _alpha = 1;
    _beta = 1;
}

bool KernelGeneratorMatmul::syncInput(void)
{
    // even if A and B do not change, C should be reset
    // to simplify detection of bad output
    return _A->flushToDevice() && _B->flushToDevice() && _C->flushToDevice();
}

bool KernelGeneratorMatmul::syncOutput(void)
{
    return _C->flushFromDevice();
}

bool KernelGeneratorMatmul::checkOutput(void)
{
    if (NO_CHECK == _checkOutputMode)
    {
        return true;
    }
    else if (SENTINEL_CHECK == _checkOutputMode)
    {
        const map<double, size_t> histMap = _C->arrayHistogram();

        // reset array for next enqueued kernels
        if (general())
            _C->fillOnes();
        else
            _C->fillZeros();

        if (1 == histMap.size())
        {
            const double sentinelValue = general()
                                             ? K() + 1
                                             : K();

            return sentinelValue == (*histMap.begin()).first;
        }
    }
    else if (PARANOID_CHECK == _checkOutputMode)
    {
        double accumC = 0;

        // reference calculation
        for (size_t b = 0; b < batching(); b++)
        for (size_t i = 0; i < M(); i++)
        for (size_t j = 0; j < N(); j++)
        {
            double valueC = 0;

            const size_t slotA = getSameDataMatrixA() ? 0 : b;
            const size_t slotB = getSameDataMatrixB() ? 0 : b;

            if (transposeA())
            {
                if (transposeB())
                {
                    for (size_t k = 0; k < K(); k++)
                    {
                        valueC += _A->getArrayElem(slotA, k, i) *
                                  _B->getArrayElem(slotB, j, k);
                    }
                }
                else
                {
                    for (size_t k = 0; k < K(); k++)
                    {
                        valueC += _A->getArrayElem(slotA, k, i) *
                                  _B->getArrayElem(slotB, k, j);
                    }
                }
            }
            else
            {
                if (transposeB())
                {
                    for (size_t k = 0; k < K(); k++)
                    {
                        valueC += _A->getArrayElem(slotA, i, k) *
                                  _B->getArrayElem(slotB, j, k);
                    }
                }
                else
                {
                    for (size_t k = 0; k < K(); k++)
                    {
                        valueC += _A->getArrayElem(slotA, i, k) *
                                  _B->getArrayElem(slotB, k, j);
                    }
                }
            }

            if (general())
            {
                valueC = _alpha * valueC
                       + _beta * _C->getHostArrayElem(b, i, j);
            }

            _C->setHostArrayElem(b, i, j, valueC);

            accumC += valueC;
        }

        // aggregate L1 error in calculation
        const double absdiffError = _C->absdiffHostArray();
        if (absdiffError >= _paranoidLimit)
        {
#ifdef __LOGGING_ENABLED__
            map< int, size_t > errHist = _C->absdiffHistogram();

            stringstream ss;
            ss << "absolute error: " << absdiffError << endl
               << "              1. <= error     : " << errHist[1] << endl
               << "            0.1 <= error < 1. : " << errHist[0] << endl
               << "          0.01 <= error < 0.1 : " << errHist[-1] << endl
               << "        0.001 <= error < 0.01 : " << errHist[-2] << endl
               << "      0.0001 <= error < 0.001 : " << errHist[-3] << endl
               << "    0.00001 <= error < 0.0001 : " << errHist[-4] << endl
               << "  0.000001 <= error < 0.00001 : " << errHist[-5] << endl
               << "0.0000001 <= error < 0.000001 : " << errHist[-6] << endl
               << "            error < 0.0000001 : " << errHist[-7];
            LOGGER(ss.str())
#endif

            return false;
        }

        // extra statistical test for floating point
        if ( PrecType::isTypeFP(precA()) &&
             PrecType::isTypeFP(precB()) &&
             PrecType::isTypeFP(precC()) )
        {
            // Elements of A and B are uniform on [0, 1) so mean value of C
            // is predictable.
            // simple matrix multiply: .5 * .5 * K
            // general matrix multiply: alpha * .5 * .5 * K + beta * .5
            const double expectMean = general()
                             ? _alpha * .25f * K() + _beta * .5f
                             : .25f * K();

            const double actualMean = accumC / (batching() * M() * N());

            // percentage error from statistically expected value, this detects
            // uninitialized buffer failures as they are usually filled with
            // zeros
            const double mape = fabs((actualMean - expectMean) / expectMean);

            // MAPE of 50 basis points seems like a lot but for small matrices
            // is reasonable, for example, 10 basis points is often exceeded
            if (mape < 0.5f)
            {
                return true;
            }
#ifdef __LOGGING_ENABLED__
            else
            {
                map< int, size_t > errHist = _C->absdiffHistogram();

                stringstream ss;
                ss << "absolute error: " << absdiffError << endl
                   << "              1. <= error     : " << errHist[1] << endl
                   << "            0.1 <= error < 1. : " << errHist[0] << endl
                   << "          0.01 <= error < 0.1 : " << errHist[-1] << endl
                   << "        0.001 <= error < 0.01 : " << errHist[-2] << endl
                   << "      0.0001 <= error < 0.001 : " << errHist[-3] << endl
                   << "    0.00001 <= error < 0.0001 : " << errHist[-4] << endl
                   << "  0.000001 <= error < 0.00001 : " << errHist[-5] << endl
                   << "0.0000001 <= error < 0.000001 : " << errHist[-6] << endl
                   << "            error < 0.0000001 : " << errHist[-7];
                LOGGER(ss.str())
            }
#endif
        }
        else
        {
            return true;
        }
    }

    return false;
}

ostream& KernelGeneratorMatmul::print(ostream& os) const
{
    const size_t vnumA = blockW() / effVecLengthA();
    const size_t vnumB = blockW() / effVecLengthB();
    const size_t vnumC = blockW() / effVecLengthC();

    const size_t batchIdxDisableA = getSameDataMatrixA() ? 0 : 1;
    const size_t batchIdxDisableB = getSameDataMatrixB() ? 0 : 1;

    ////////////////////////////////////////
    // OpenCL double precision extension
    if ( PrecType::Double == precA() ||
         PrecType::Double == precB() ||
         PrecType::Double == precC() )
    {
        os << pragma_extension(PrecType::Double, _deviceIndex);
    }

    ////////////////////////////////////////
    // kernel arguments
    FunctionDeclaration kDecl(kernelName());

    const GlobalVar matrixC("matrixC", precC(), vecLengthC(), true, kDecl);
    const GlobalVar matrixA("matrixA", precA(), vecLengthA(), false, kDecl);
    const GlobalVar matrixB("matrixB", precB(), vecLengthB(), false, kDecl);

    const LocalVar localA("localA", precA(), vecLengthA(), true, kDecl,
                          0 != vecLengthA());
    const LocalVar localB("localB", precB(), vecLengthB(), true, kDecl,
                          0 != vecLengthB());

    const PrivateVar varM("M", PrecType::Int32, 1, false, kDecl,
                          M(), inlineDim());
    const PrivateVar varN("N", PrecType::Int32, 1, false, kDecl,
                          N(), inlineDim());
    const PrivateVar varK("K", PrecType::Int32, 1, false, kDecl,
                          K(), inlineDim());

    const PrivateVar alpha("alpha",
                           max<size_t>(precA(), precB()),
                           1,
                           false,
                           kDecl,
                           general());

    const PrivateVar beta("beta",
                          precC(),
                          1,
                          false,
                          kDecl,
                          general());

    // declare kernel
    os << kDecl;

    ////////////////////////////////////////
    // image sampler
    const SamplerVar sampler;
    if (0 == vecLengthA() || 0 == vecLengthB())
    {
        os << declare(sampler, ImageSampler());
    }

    ////////////////////////////////////////
    // pointers to memory buffers
    const GlobalVar ptrMatrixA("ptrMatrixA", precA(), vecLengthA(), false);
    const LocalVar ptrLocalA("ptrLocalA", precA(), vecLengthA(), false);
    if (0 != vecLengthA())
    {
        os << declare(ptrMatrixA);
        os << declare(ptrLocalA);
    }
    const GlobalVar ptrMatrixB("ptrMatrixB", precB(), vecLengthB(), false);
    const LocalVar ptrLocalB("ptrLocalB", precB(), vecLengthB(), false);
    if (0 != vecLengthB())
    {
        os << declare(ptrMatrixB);
        os << declare(ptrLocalB);
    }

    ////////////////////////////////////////
    // accumulate inner product sum
    const size_t accumPrec = max<size_t>(precA(), precB(), precC());
    const vector< vector< PrivateVar > > accum = privateVector(
                                                     "accum",
                                                     accumPrec,
                                                     vecLengthC(),
                                                     blockH(),
                                                     vnumC);
    os << declare(accum);

    ////////////////////////////////////////
    // registers for inner product
    const vector< vector< PrivateVar > > valueA = privateVector(
                                                      "valueA",
                                                      precA(),
                                                      vecLengthA(),
                                                      blockH(),
                                                      vnumA);
    const vector< vector< PrivateVar > > valueB = privateVector(
                                                      "valueB",
                                                      precB(),
                                                      vecLengthB(),
                                                      blockW(),
                                                      vnumB);
    os << declare(valueA)
       << declare(valueB);

    ////////////////////////////////////////
    // begin packing loop
    const PrivateVar batchIdx("batchIdx", PrecType::Int32, 1, true,
                              0, 1 == batching());
    if (1 != batching())
        os << ForLoop(batchIdx, batching(), 1);

    // set accumulators to zero
    os << assign(accum, CastValue(ConstantValue(0), accumPrec, vecLengthC()));

    // initialize pointer to memory buffer for matrix A
    if (0 != vecLengthA())
    {
        // start at left or top edge of matrix A
        if (transposeA())
        {
            // start at top edge of matrix A (height is K, width is M)
            // (must transpose quads into local memory)
            os << assign( ptrMatrixA,

                matrixA

                // down by packed KxM matrices
                + batchIdxDisableA * (batchIdx * (varM * varK / vecLengthA()))

                // across by global index
                + globalRow * (blockH() / vecLengthA())

                // down by local index
                + localCol * (varM * (blockW() / vecLengthA())) );
        }
        else
        {
            // start at left edge of matrix A (height is M, width is K)
            os << assign( ptrMatrixA,

                matrixA

                // down by packed MxK matrices
                + batchIdxDisableA * (batchIdx * (varM * varK / vecLengthA()))

                // down by work group
                + groupRow *
                  ( 0 == (groupSize() * blockH()) % vecLengthA()
                        ? varK * (groupSize() * blockH() / vecLengthA())
                        : (groupSize() * blockH()) * varK / vecLengthA() )

                // down by local index
                + localRow * (varK / vecLengthA())

                // across by local index
                + localCol );
        }
    }

    // initialize pointer to memory buffer for matrix B
    if (0 != vecLengthB())
    {
        // start at left or top edge of matrix B
        if (transposeB())
        {
            // start at left edge of matrix B (height is N, width is K)
            os << assign( ptrMatrixB,

                matrixB

                // down by packed NxK matrices
                + batchIdxDisableB * (batchIdx * (varN * varK / vecLengthB()))

                // down by work group
                + groupCol * (varK * (groupSize() * blockW() / vecLengthB()))

                // down by local index
                + localRow * (varK / vecLengthB())

                // across by local index
                + localCol );
        }
        else
        {
            // start at top edge of matrix B (height is K, width is N)
            // (must transpose quads into local memory)
            os << assign( ptrMatrixB,

                matrixB

                // down by packed KxN matrices
                + batchIdxDisableB * (batchIdx * (varN * varK / vecLengthB()))

                // across by global index
                + globalCol * (blockW() / vecLengthB())

                // down by local index
                + localRow * (varN * (blockW() / vecLengthB())) );
        }
    }

    ////////////////////////////////////////
    // begin outer loop over blocks
    const PrivateVar outerIdx("outerIdx", PrecType::Int32, 1, true);
    if (0 != vecLengthA() || 0 != vecLengthB())
    {
        os << ForLoop(outerIdx, varK / (groupSize() * blockW()), 1);
    }

    // copy from global memory buffer for matrix A to local memory
    if (0 != vecLengthA())
    {
        for (size_t i = 0; i < blockH(); i++)
        for (size_t j = 0; j < vnumA; j++)
        {
            if (transposeA())
            {
                const size_t blockNum = i / vecLengthA();
                const size_t blockIdx = i % vecLengthA();

                os << assign( LocalVar(

                    *(localA + (localRow * (blockH() * localSize()) +
                                localCol +
                                i * localSize()) * vnumA
                             + j) ),

                    *(ptrMatrixA + blockNum
                                 + blockIdx * varM / vecLengthA()
                                 + j * varM) );
            }
            else
            {
                os << assign( LocalVar(

                    *(localA + localRow * (localSize() * vnumA)
                             + i * groupSize() * localSize() * vnumA
                             + localCol
                             + j * groupSize()) ),

                    *(ptrMatrixA + (i * groupSize()) * varK / vecLengthA()
                                 + j * groupSize()) );
            }
        }
    }

    // copy from global memory buffer for matrix B to local memory
    if (0 != vecLengthB())
    {
        for (size_t i = 0; i < blockW(); i++)
        for (size_t j = 0; j < vnumB; j++)
        {
            if (transposeB())
            {
                os << assign( LocalVar(

                    *(localB + localRow * (localSize() * vnumB)
                             + i * groupSize() * localSize() * vnumB
                             + localCol
                             + j * groupSize()) ),

                    *(ptrMatrixB + (i * groupSize()) * varK / vecLengthB()
                                 + j * groupSize()) );
            }
            else
            {
                os << assign( LocalVar(

                    *(localB + (localCol * (blockW() * localSize()) +
                                localRow) * vnumB
                             + i * localSize() * vnumB
                             + j) ),

                    *(ptrMatrixB + i * varN / vecLengthB() + j) );
            }
        }
    }

    // need barrier if using local memory
    if (0 != vecLengthA() || 0 != vecLengthB())
    {
        os << LocalBarrier();
    }

    // update global memory buffer pointer for matrix A
    if (0 != vecLengthA())
    {
        if (transposeA())
        {
            os << increment( ptrMatrixA, varM * (groupSize() * vnumA) );
        }
        else
        {
            os << increment( ptrMatrixA, groupSize() * vnumA );
        }
    }

    // update global memory buffer pointer for matrix B
    if (0 != vecLengthB())
    {
        if (transposeB())
        {
            os << increment( ptrMatrixB, groupSize() * vnumB );
        }
        else
        {
            os << increment( ptrMatrixB, varN * (groupSize() * vnumB) );
        }
    }

    // initialize inner product pointer when using memory buffer for matrix A
    if (0 != vecLengthA())
    {
        os << assign( ptrLocalA,
                      localA + localRow * (localSize() * blockH() * vnumA) );
    }

    // initialize inner product pointer when using memory buffer for matrix B
    if (0 != vecLengthB())
    {
        os << assign( ptrLocalB,
                      localB + localCol * (localSize() * blockW() * vnumB) );
    }

    const PrivateVar kIdx("kIdx", PrecType::Int32, 1, true);
    const PrivateVar innerIdx("innerIdx", PrecType::Int32, 1, true);
    if (0 == vecLengthA() && 0 == vecLengthB())
    {
        ////////////////////////////////////////
        // one loop for A and B both images
        os << ForLoop(kIdx, varK / blockW(), 1);
    }
    else
    {
        ////////////////////////////////////////
        // begin inner product accumulation
        os << ForLoop(innerIdx, groupSize(), 1);
    }

    // read in values of matrix A
    if (0 == vecLengthA())
    {
        // image
        for (size_t i = 0; i < blockH(); i++)
        for (size_t j = 0; j < vnumA; j++)
        {
            const size_t veclength = effVecLengthA();

            const PrivateVar idx( 0 == vecLengthB()
                                      ? ConstantValue(kIdx)
                                      : outerIdx * groupSize() + innerIdx );

            if (transposeA())
            {
                const size_t blockNum = i / veclength;
                const size_t blockIdx = i % veclength;

                os << assign(
                    valueA[j][i],
                    ReinterpretValue(
                        ReadImage(
                            matrixA,
                            sampler,
                            globalRow * (blockH() / veclength) + blockNum,
                            batchIdxDisableA * (batchIdx * varK)
                                + idx * blockW()
                                + (j * veclength + blockIdx) ),
                        precA(),
                        vecLengthA(),
                        PrecType::Double == precA() ));
            }
            else
            {
                os << assign(
                    valueA[j][i],
                    ReinterpretValue(
                        ReadImage(
                            matrixA,
                            sampler,
                            idx * vnumA + j,
                            batchIdxDisableA * (batchIdx * varM)
                                + globalRow * blockH()
                                + i ),
                        precA(),
                        vecLengthA(),
                        PrecType::Double == precA() ));
            }
        }
    }
    else
    {
        // memory buffer
        for (size_t i = 0; i < blockH(); i++)
        for (size_t j = 0; j < vnumA; j++)
        {
            os << assign( valueA[j][i],
                          *(ptrLocalA + i * localSize() * vnumA + j) );
        }

        os << increment( ptrLocalA, vnumA );
    }

    // read in values of matrix B
    if (0 == vecLengthB())
    {
        // image
        for (size_t i = 0; i < blockW(); i++)
        for (size_t j = 0; j < vnumB; j++)
        {
            const size_t veclength = effVecLengthB();

            const PrivateVar idx( 0 == vecLengthA()
                                      ? ConstantValue(kIdx)
                                      : outerIdx * groupSize() + innerIdx );

            if (transposeB())
            {
                os << assign(
                    valueB[j][i],
                    ReinterpretValue(
                        ReadImage(
                            matrixB,
                            sampler,
                            idx * vnumB + j,
                            batchIdxDisableB * (batchIdx * varN)
                                + globalCol * blockW()
                                + i ),
                        precB(),
                        vecLengthB(),
                        PrecType::Double == precB() ));
            }
            else
            {
                os << assign(
                    valueB[j][i],
                    ReinterpretValue(
                        ReadImage(
                            matrixB,
                            sampler,
                            globalCol * vnumB + j,
                            batchIdxDisableB * (batchIdx * varK)
                                + idx * blockW()
                                + i ),
                        precB(),
                        vecLengthB(),
                        PrecType::Double == precB() ));
            }
        }
    }
    else
    {
        // memory buffer
        for (size_t i = 0; i < blockW(); i++)
        for (size_t j = 0; j < vnumB; j++)
        {
            os << assign( valueB[j][i],
                          *(ptrLocalB + i * localSize() * vnumB + j) );
        }

        os << increment( ptrLocalB, vnumB );
    }

    // assignMAD goes here
    assignMAD(os, loopOrder(), accum, valueA, valueB);

    if (0 == vecLengthA() && 0 == vecLengthB())
    {
        ////////////////////////////////////////
        // end one loop for A and B both images
        os << EndBlock();
    }
    else
    {
        ////////////////////////////////////////
        // end inner product accumulation
        os << EndBlock();

        ////////////////////////////////////////
        // end outer loop over blocks
        os << EndBlock();
    }

    const PrivateVar switchGlobalRow(
                         globalID() ? globalRow
                                    : groupRow * groupSize() + localRow );

    const PrivateVar switchGlobalCol(
                         globalID() ? globalCol
                                    : groupCol * groupSize() + localCol );

    if (0 == vecLengthC())
    {
        const size_t veclength = effVecLengthC();

        // image
        for (size_t i = 0; i < blockH(); i++)
        for (size_t j = 0; j < vnumC; j++)
        {
            os << WriteImage(
                      matrixC,
                      switchGlobalCol * vnumC + j,
                      batchIdx * varM
                          + switchGlobalRow * blockH()
                          + i,
                      ReinterpretValue(
                          ConvertValue(
                              accum[j][i],
                              precC(),
                              vecLengthC(),
                              accumPrec != precC() ),
                          PrecType::UInt32,
                          PrecType::vecLength(PrecType::UInt32),
                          PrecType::Double == precC() ));
        }
    }
    else
    {
        // memory buffer
        const GlobalVar ptrMatrixC(
                            matrixC
                            + batchIdx * (varM * varN / vecLengthC())
                            + switchGlobalRow * (blockH() * varN / vecLengthC())
                            + switchGlobalCol * (blockW() / vecLengthC()) );

        for (size_t i = 0; i < blockH(); i++)
        for (size_t j = 0; j < vnumC; j++)
        {
            os << assign(
                GlobalVar(*(ptrMatrixC + i * varN / vecLengthC() + j)),
                ConvertValue(
                    general()
                        ? MADValue(CastValue(alpha, accumPrec, vecLengthC()),
                                   accum[j][i],
                                   CastValue(beta, accumPrec, vecLengthC()) *
                                   *(ptrMatrixC + i * varN / vecLengthC() + j),
                                   useMADFunction())
                        : accum[j][i],
                    precC(),
                    vecLengthC(),
                    accumPrec != precC() ));
        }
    }

    ////////////////////////////////////////
    // end packing loop
    if (1 != batching())
        os << EndBlock();

    return os << EndBlock();
}

}; // namespace chai_internal
