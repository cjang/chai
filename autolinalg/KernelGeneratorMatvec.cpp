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
#include "KernelGeneratorMatvec.hpp"
#include "Logger.hpp"
#include "PrecType.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

using namespace AutoParamsMatmul;

////////////////////////////////////////
// GEMV kernel generator

bool KernelGeneratorMatvec::setArgsInternal(OCLkernel& ckernel,
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

    // set local memory for array B
    if (_B->isMembuf())
    {
        const size_t numberElems = transposeA()
                                       ? localSize()
                                       : localSize() * blockW();

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

KernelGeneratorMatvec::KernelGeneratorMatvec(const size_t deviceIndex)
    : ParamInlineDim(getExtraParam()),
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

KernelGeneratorMatvec::~KernelGeneratorMatvec(void)
{
    delete _A;
    delete _B;
    delete _C;
}

void KernelGeneratorMatvec::noCheck(void)
{
    _checkOutputMode = NO_CHECK;
    _checkOutputTouched = true;
}

void KernelGeneratorMatvec::sentinelCheck(void)
{
    _checkOutputMode = SENTINEL_CHECK;
    _checkOutputTouched = true;
}

void KernelGeneratorMatvec::paranoidCheck(const double absDiffLimit)
{
    _checkOutputMode = PARANOID_CHECK;
    _checkOutputTouched = true;
    _paranoidLimit   = absDiffLimit;
}

bool KernelGeneratorMatvec::setArgs(OCLdevice& cdev,
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
                               N(),
                               getSameDataMatrixA() ? 1 : batching())
                : new ArrayBuf(cdev,
                               ArrayBuf::READ,
                               precA(),
                               vecLengthA(),
                               N(),
                               M(),
                               getSameDataMatrixA() ? 1 : batching());

        B = new ArrayBuf(cdev,
                         ArrayBuf::READ,
                         precB(),
                         vecLengthB(),
                         N(),
                         1,
                         getSameDataMatrixB() ? 1 : batching());

        C = new ArrayBuf(cdev,
                         general() ? ArrayBuf::READWRITE : ArrayBuf::WRITE,
                         precC(),
                         vecLengthC(),
                         M(),
                         1,
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

bool KernelGeneratorMatvec::setArgs(OCLkernel& ckernel,
                                    ArrayBuf* A,
                                    ArrayBuf* B,
                                    ArrayBuf* C,
                                    const double alpha,
                                    const double beta)
{
    return setArgsInternal(ckernel, A, B, C, alpha, beta);
}

void KernelGeneratorMatvec::clearArgs(void)
{
    // do not delete array buffer objects, they are owned by the caller
    _A = NULL;
    _B = NULL;
    _C = NULL;

    _alpha = 1;
    _beta = 1;
}

bool KernelGeneratorMatvec::syncInput(void)
{
    // even if A and B do not change, C should be reset
    // to simplify detection of bad output
    return _A->flushToDevice() && _B->flushToDevice() && _C->flushToDevice();
}

bool KernelGeneratorMatvec::syncOutput(void)
{
    return _C->flushFromDevice();
}

bool KernelGeneratorMatvec::checkOutput(void)
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
                                             ? N() + 1
                                             : N();

            return sentinelValue == (*histMap.begin()).first;
        }
    }
    else if (PARANOID_CHECK == _checkOutputMode)
    {
        double accumC = 0;

        // reference calculation
        for (size_t b = 0; b < batching(); b++)
        for (size_t j = 0; j < M(); j++)
        {
            double valueC = 0;

            const size_t slotA = getSameDataMatrixA() ? 0 : b;
            const size_t slotB = getSameDataMatrixB() ? 0 : b;

            if (transposeA())
            {
                for (size_t l = 0; l < N(); l++)
                {
                    valueC += _A->getArrayElem(slotA, l, j) *
                              _B->getArrayElem(slotB, 0, l);
                }
            }
            else
            {
                for (size_t l = 0; l < N(); l++)
                {
                    valueC += _A->getArrayElem(slotA, j, l) *
                              _B->getArrayElem(slotB, 0, l);
                }
            }

            if (general())
            {
                valueC = _alpha * valueC
                       + _beta * _C->getHostArrayElem(b, 0, j);
            }

            _C->setHostArrayElem(b, 0, j, valueC);

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
            // simple matrix-vector multiply: .5 * .5 * N
            // general matrix-vector multiply: alpha * .5 * .5 * N + beta * .5
            const double expectMean = general()
                             ? _alpha * .25f * N() + _beta * .5f
                             : .25f * N();

            const double actualMean = accumC / (batching() * M());

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

ostream& KernelGeneratorMatvec::print(ostream& os) const
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

    const LocalVar localB("localB", precB(), vecLengthB(), true, kDecl,
                          0 != vecLengthB());

    const PrivateVar varM("M", PrecType::Int32, 1, false, kDecl,
                          M(), inlineDim());
    const PrivateVar varN("N", PrecType::Int32, 1, false, kDecl,
                          N(), inlineDim());

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
        os << declare(sampler, ImageSampler());

    ////////////////////////////////////////
    // pointers to memory buffers
    const GlobalVar ptrMatrixA("ptrMatrixA", precA(), vecLengthA(), false);
    if (0 != vecLengthA())
        os << declare(ptrMatrixA);
    const GlobalVar ptrMatrixB("ptrMatrixB", precB(), vecLengthB(), false);
    if (0 != vecLengthB())
        os << declare(ptrMatrixB);

    ////////////////////////////////////////
    // accumulate inner product sum
    const size_t accumPrec = max<size_t>(precA(), precB(), precC());
    const vector< PrivateVar > accum = privateVector(
                                           "accum",
                                           accumPrec,
                                           vecLengthC(),
                                           transposeA() ? vnumC : 1 );
    os << declare(accum);

    ////////////////////////////////////////
    // registers for inner product
    const vector< PrivateVar > valueA = privateVector(
                                            "valueA",
                                            precA(),
                                            vecLengthA(),
                                            vnumA );
    const vector< PrivateVar > valueB = privateVector( 
                                            "valueB",
                                            precB(),
                                            vecLengthB(),
                                            transposeA() ? 1 : vnumB );
    os << declare(valueA)
       << declare(valueB);

    ////////////////////////////////////////
    // begin batching loop
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
            // start at top edge of matrix A (height is N, width is M)
            // output for this ID is blockW() elements
            os << assign( ptrMatrixA,

                matrixA

                // down by packed NxM matrices
                + batchIdxDisableA * (batchIdx * (varM * varN / vecLengthA()))

                // across by global index
                + globalCol * vnumA );
        }
        else
        {
            // start at left edge of matrix A (height is M, width is N)
            // output for this ID is effVecLengthC() elements
            os << assign( ptrMatrixA,

                matrixA

                // down by packed MxN matrices
                + batchIdxDisableA * (batchIdx * (varM * varN / vecLengthA()))

                // down by global index
                + globalCol * (effVecLengthC() * varN / vecLengthA()) );
        }
    }

    // initialize pointer to memory buffer for matrix B
    if (0 != vecLengthB())
    {
        // start at top edge of matrix B (height is 1, width is N)
        os << assign( ptrMatrixB,

            matrixB

            // down by packed 1xN matrices
            + batchIdxDisableB * (batchIdx * (varN / vecLengthB()))

            // across by local index
            + localCol );
    }

    ////////////////////////////////////////
    // reduction loop
    const PrivateVar outerIdx("outerIdx", PrecType::Int32, 1, true);
    if (transposeA()) // A is transposed, column major
    {
        if (0 == vecLengthB())
        {
            ////////////////////////////////////////
            // A transposed, B image

            // B is image so iterate directly over elements
            os << ForLoop(outerIdx, varN / effVecLengthB(), 1);

            // read in value of B
            os << assign( valueB[0],
                          ReinterpretValue(
                              ReadImage(
                                  matrixB,
                                  sampler,
                                  outerIdx,
                                  batchIdxDisableB * batchIdx ),
                              precB(),
                              vecLengthB(),
                              PrecType::Double == precB() ));

            // iterate over rows of A within the vector length of B
            for (size_t oIdx = 0; oIdx < effVecLengthB(); oIdx++)
            {
                // read in one block from A
                for (size_t i = 0; i < vnumA; i++)
                {
                    if (0 == vecLengthA())
                    {
                        // A is image
                        os << assign( valueA[i],
                            ReinterpretValue(
                                ReadImage(
                                    matrixA,
                                    sampler,
                                    globalCol * vnumA + i,
                                    batchIdxDisableA * (batchIdx * varN)
                                        + outerIdx * effVecLengthB()
                                        + oIdx ),
                                precA(),
                                vecLengthA(),
                                PrecType::Double == precA() ));
                    }
                    else
                    {
                        // A is memory buffer
                        os << assign( valueA[i], *(ptrMatrixA + i) );
                    }
                }

                // next row of A if memory buffer
                if (0 != vecLengthA())
                    os << increment( ptrMatrixA, varM / vecLengthA() );

                // accumulate
                for (size_t j = 0; j < blockW(); j++)
                    os << assignMAD(accum, valueA, valueB, j, oIdx);
            }
        }
        else
        {
            ////////////////////////////////////////
            // A transposed, B memory buffer

            // B is memory buffer so need inner blocking to read into local
            os << ForLoop(outerIdx, varN / (groupSize() * effVecLengthB()), 1);

            // copy from global memory buffer for matrix B to local memory
            os << assign( LocalVar( *(localB + localCol) ), *ptrMatrixB );
            os << increment( ptrMatrixB, groupSize() );

            // need barrier if using local memory
            os << LocalBarrier();

            // iterate over block in the local memory for B
            for (size_t blockIdx = 0; blockIdx < groupSize(); blockIdx++)
            {
                // read in value from B
                os << assign( valueB[0], *(localB + blockIdx) );

                // each component of value in B
                for (size_t oIdx = 0; oIdx < effVecLengthB(); oIdx++)
                {
                    // read in one block from A
                    for (size_t i = 0; i < vnumA; i++)
                    {
                        if (0 == vecLengthA())
                        {
                            // A is image
                            os << assign( valueA[i],
                                ReinterpretValue(
                                    ReadImage(
                                        matrixA,
                                        sampler,
                                        globalCol * vnumA + i,
                                        batchIdxDisableA * (batchIdx * varN)
                                            + outerIdx * ( groupSize()
                                                         * effVecLengthB() )
                                            + blockIdx * effVecLengthB()
                                            + oIdx ),
                                    precA(),
                                    vecLengthA(),
                                    PrecType::Double == precA() ));
                        }
                        else
                        {
                            // A is memory buffer
                            os << assign( valueA[i], *(ptrMatrixA + i) );
                        }
                    }

                    // next row of A if memory buffer
                    if (0 != vecLengthA())
                        os << increment( ptrMatrixA, varM / vecLengthA() );

                    // accumulate
                    for (size_t j = 0; j < blockW(); j++)
                        os << assignMAD(accum, valueA, valueB, j, oIdx);
                }
            }
        }

        // end outer loop over group blocks
        os << EndBlock(); // outerIdx

        const PrivateVar switchGlobalCol(
                             globalID()
                                 ? globalCol
                                 : groupCol * groupSize() + localCol );

        // write to output vector C
        for (size_t i = 0; i < vnumC; i++)
        {
            if (0 == vecLengthC())
            {
                os << WriteImage(
                          matrixC,
                          switchGlobalCol * vnumC + i,
                          batchIdx * varN,
                          ReinterpretValue(
                              ConvertValue(
                                  accum[i],
                                  precC(),
                                  vecLengthC(),
                                  accumPrec != precC() ),
                              PrecType::UInt32,
                              PrecType::vecLength(PrecType::UInt32),
                              PrecType::Double == precC() ));
            }
            else
            {
                const GlobalVar ptrMatrixC( matrixC
                                            + batchIdx * (varM / vecLengthC())
                                            + switchGlobalCol * vnumC
                                            + i );

                os << assign(
                    GlobalVar(*ptrMatrixC),
                    ConvertValue(
                        general()
                            ? MADValue(
                                  CastValue(alpha, accumPrec, vecLengthC()),
                                  accum[i],
                                  CastValue(beta, accumPrec, vecLengthC()) *
                                  *ptrMatrixC,
                                  useMADFunction())
                            : accum[i],
                        precC(),
                        vecLengthC(),
                        accumPrec != precC() ));
            }
        }
    }
    else // A is not transposed, row major
    {
        if (0 == vecLengthB())
        {
            ////////////////////////////////////////
            // A row major, B image

            // B is image so just iterate directly over blocks
            os << ForLoop(outerIdx, varN / blockW(), 1);

            // read in one block from B
            for (size_t i = 0; i < vnumB; i++)
            {
                os << assign( valueB[i],
                          ReinterpretValue(
                              ReadImage(
                                  matrixB,
                                  sampler,
                                  outerIdx * vnumB + i,
                                  batchIdxDisableB * batchIdx ),
                              precB(),
                              vecLengthB(),
                              PrecType::Double == precB() ));
            }

            // each component of output vector C corresponds to a row of A
            for (size_t oIdx = 0; oIdx < effVecLengthC(); oIdx++)
            {
                // read in one block from A
                for (size_t i = 0; i < vnumA; i++)
                {
                    if (0 == vecLengthA())
                    {
                        // A is image
                        os << assign( valueA[i],
                            ReinterpretValue(
                                ReadImage(
                                    matrixA,
                                    sampler,
                                    outerIdx * vnumA + i,
                                    batchIdxDisableA * (batchIdx * varM)
                                        + globalCol * effVecLengthC()
                                        + oIdx ),
                                precA(),
                                vecLengthA(),
                                PrecType::Double == precA() ));
                    }
                    else
                    {
                        // A is memory buffer
                        os << assign( valueA[i],
                                      *(ptrMatrixA + oIdx * varN / vecLengthA()
                                                   + outerIdx * vnumA
                                                   + i) );
                    }
                }

                // accumulate
                for (size_t l = 0; l < blockW(); l++)
                    os << assignMAD(accum, valueA, valueB, oIdx, l);
            }
        }
        else
        {
            ////////////////////////////////////////
            // A row major, B memory buffer

            // B is memory buffer so need inner blocking to read into local
            os << ForLoop(outerIdx, varN / (groupSize() * blockW()), 1);

            // copy from global memory buffer for matrix B to local memory
            for (size_t i = 0; i < vnumB; i++)
            {
                os << assign(
                          LocalVar( *(localB + localCol + i * groupSize()) ),
                          *ptrMatrixB );

                os << increment( ptrMatrixB, groupSize() );
            }

            // need barrier if using local memory
            os << LocalBarrier();

            // iterate over multiple blocks in the local memory for B
            for (size_t blockIdx = 0; blockIdx < groupSize(); blockIdx++)
            {
                // read in one block from B
                for (size_t i = 0; i < vnumB; i++)
                {
                    os << assign( valueB[i],
                                  *(localB + blockIdx * vnumB + i) );
                }

                // each component of output vector C corresponds to a row of A
                for (size_t oIdx = 0; oIdx < effVecLengthC(); oIdx++)
                {
                    // read in one block from A
                    for (size_t i = 0; i < vnumA; i++)
                    {
                        if (0 == vecLengthA())
                        {
                            // A is image
                            os << assign( valueA[i],
                                ReinterpretValue(
                                    ReadImage(
                                        matrixA,
                                        sampler,
                                        outerIdx * (groupSize() * vnumA)
                                            + blockIdx * vnumA
                                            + i,
                                        batchIdxDisableA * (batchIdx * varM)
                                            + globalCol * effVecLengthC()
                                            + oIdx ),
                                    precA(),
                                    vecLengthA(),
                                    PrecType::Double == precA() ));
                        }
                        else
                        {
                            // A is memory buffer
                            os << assign( valueA[i],
                                *(ptrMatrixA + oIdx * varN / vecLengthA()
                                             + outerIdx * (groupSize() * vnumA)
                                             + blockIdx * vnumA
                                             + i) );
                        }
                    }

                    // accumulate
                    for (size_t l = 0; l < blockW(); l++)
                        os << assignMAD(accum, valueA, valueB, oIdx, l);
                }
            }
        }

        // end outer loop over group blocks
        os << EndBlock(); // outerIdx

        const PrivateVar switchGlobalCol(
                             globalID()
                                 ? globalCol
                                 : groupCol * groupSize() + localCol );

        // write to output vector C
        if (0 == vecLengthC())
        {
            os << WriteImage(
                      matrixC,
                      switchGlobalCol,
                      batchIdx * varM,
                      ReinterpretValue(
                          ConvertValue(
                              accum[0],
                              precC(),
                              vecLengthC(),
                              accumPrec != precC() ),
                          PrecType::UInt32,
                          PrecType::vecLength(PrecType::UInt32),
                          PrecType::Double == precC() ));
        }
        else
        {
            const GlobalVar ptrMatrixC( matrixC
                                        + batchIdx * (varM / vecLengthC())
                                        + switchGlobalCol );

            os << assign(
                GlobalVar(*ptrMatrixC),
                ConvertValue(
                    general()
                        ? MADValue(CastValue(alpha, accumPrec, vecLengthC()),
                                   accum[0],
                                   CastValue(beta, accumPrec, vecLengthC()) *
                                   *ptrMatrixC,
                                   useMADFunction())
                        : accum[0],
                    precC(),
                    vecLengthC(),
                    accumPrec != precC() ));
        }
    }

    ////////////////////////////////////////
    // end batching loop
    if (1 != batching())
        os << EndBlock();

    return os << EndBlock();
}

}; // namespace chai_internal
