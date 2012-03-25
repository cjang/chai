// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <map>
#include <math.h>
#include <sstream>

#include "ArrayBufUtil.hpp"
#include "CodeControlFlow.hpp"
#include "CodeFunction.hpp"
#include "CodeImageSample.hpp"
#include "CodeStatements.hpp"
#include "CodeType.hpp"
#include "CodeValues.hpp"
#include "EvergreenMatmulMV.hpp"
#include "Logger.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

namespace Evergreen {

////////////////////////////////////////
// Evergreen::MatmulMVBase

MatmulMVBase::MatmulMVBase(void)
    : MatmulBase() { }

MatmulMVBase::~MatmulMVBase(void) { }

string MatmulMVBase::assignMAD(
                         const vector< PrivateVar >& accum,
                         const vector< PrivateVar >& valueA,
                         const vector< PrivateVar >& valueB,
                         const size_t j,        // output row
                         const size_t l ) const // inner product column
{
    const size_t vlenA = valueA[0].vectorLength();
    const size_t vlenB = valueB[0].vectorLength();
    const size_t vlenC = accum[0].vectorLength();

    if (transposeA())
    {
        // At B
        return assign( accum[j / vlenC][j % vlenC],
                       MADValue( valueA[j / vlenA][j % vlenA],
                                 valueB[l / vlenB][l % vlenB],
                                 accum[j / vlenC][j % vlenC],
                                 useMADFunction() ));
    }
    else
    {
        // A B
        return assign( accum[j / vlenC][j % vlenC],
                       MADValue( valueA[l / vlenA][l % vlenA],
                                 valueB[l / vlenB][l % vlenB],
                                 accum[j / vlenC][j % vlenC],
                                 useMADFunction() ));
    }
}

string MatmulMVBase::kernelForename(void) const
{
    return "matvec";
}

bool MatmulMVBase::validParams(void) const
{
    bool blockingOk;

    if (transposeA())
    {
        blockingOk =

            // block width must fit along rows of transposed matrix
            // and the output vector
            0 == dimensionM() % (groupSize() * blockWidth()) &&
            groupSize() * blockWidth() <= dimensionM() &&

            // work item group size must fit input vector
            0 == dimensionN() % (groupSize() * effVectorLengthB()) &&
            groupSize() * effVectorLengthB() <= dimensionN();
    }
    else
    {
        blockingOk =

            // block width must fit along rows of matrix and the input vector
            0 == dimensionN() % (groupSize() * blockWidth()) &&
            groupSize() * blockWidth() <= dimensionN() &&

            // work item group size must fit output vector
            0 == dimensionM() % (groupSize() * effVectorLengthC()) &&
            groupSize() * effVectorLengthC() <= dimensionM();
    }

    return
        // array types of A, B, C
        validArrayType(precisionA(), vectorLengthA()) &&
        validArrayType(precisionB(), vectorLengthB()) &&
        validArrayType(precisionC(), vectorLengthC()) &&

        // if GEMV, then C must be read-write (can not be an image)
        !(general() && 0 == vectorLengthC()) &&

        // packed kernels
        packing() > 0 &&

        // inner and outer blocking compatible with problem dimensions
        blockingOk &&

        // extra parameter
        extraParam() < totalVariations();
}

bool MatmulMVBase::getParams(vector<size_t>& params) const
{
    bool rc;

    if (rc = validParams())
    {
        params.clear();

        // array precisions
        params.push_back(precisionA());
        params.push_back(precisionB());
        params.push_back(precisionC());

        // array vector lengths
        params.push_back(vectorLengthA());
        params.push_back(vectorLengthB());
        params.push_back(vectorLengthC());

        // packed kernels
        params.push_back(packing());
        if (getDifferentData()) params.push_back(0);
        else if (getSameDataMatrixA()) params.push_back(1);
        else if (getSameDataMatrixB()) params.push_back(2);

        // GEMM or pure matrix multiply only
        params.push_back(general());

        // matrix dimensions
        params.push_back(dimensionM());
        params.push_back(dimensionN());

        // data layout
        params.push_back(transposeA());

        // work group
        params.push_back(groupSize());

        // inner blocking
        params.push_back(blockWidth());

        // extra parameter
        params.push_back(extraParam());
    }

    return rc;
}

string MatmulMVBase::exogenousKey(void) const
{
    stringstream ss;

    ss << deviceFamily()
       << kernelForename()
       << packing()
       << (getSameDataMatrixA() ? "A" : "N")
       << (getSameDataMatrixB() ? "B" : "N")
       << (general() ? "G" : "N")
       << precisionA()
       << precisionB()
       << precisionC()
       << (transposeA() ? "T" : "N")
       << "_" << dimensionM()
       << "_" << dimensionN();

    return ss.str();
}

void MatmulMVBase::toggleParamMarks(void)
{
    MatmulBase::toggleParamMarks();
}

bool MatmulMVBase::getParams(set< vector<size_t> >& paramSet)
{
    // exogenous: packing (default is 1: no packing)
    //            general (default is false: pure matrix multiply)
    //            precision (default is 0, must be set)
    //            dimensions (default is 0, must be set)
    //            data layout (default is false: row major)
    //
    // endogenous: work group
    //             inner blocking
    //             vector length
    //             inline dimensions (extra parameter)
    //             global ID (extra parameter)

    const bool exogenousOk =
        (packing() > 0) &&
        PrecType::validSizeCode(precisionA()) &&
        PrecType::validSizeCode(precisionB()) &&
        PrecType::validSizeCode(precisionC()) &&
        (0 != dimensionM()) &&
        (0 != dimensionN());

    if (! exogenousOk)
        return false;

    const size_t wgStart = WorkGroup::isActive() ? 1 : 0;
    const size_t wgLimit = WorkGroup::isActive() ? 256 : 0;
    const size_t ibWidthStart = InnerBlocking::isActive() ? 1 : 0;
    const size_t ibWidthLimit = InnerBlocking::isActive() ? 64 : 0;
    const size_t vlenStart = VectorLength::isActive() ? 0 : 0;
    const size_t vlenLimit = VectorLength::isActive() ? 4 : 0;
    const size_t xParamLimit = ExtraParam::isActive() ? totalVariations() : 1;

    bool foundParams = false;

    // work group
    for (size_t wgSize = wgStart; wgSize <= wgLimit; wgSize++)

    // inner blocking
    for (size_t ibWidth = ibWidthStart; ibWidth <= ibWidthLimit; ibWidth++)

    // vector length
    for (size_t vlenA = vlenStart; vlenA <= vlenLimit; vlenA++)
    for (size_t vlenB = vlenStart; vlenB <= vlenLimit; vlenB++)
    for (size_t vlenC = vlenStart; vlenC <= vlenLimit; vlenC++)

    // extra parameter
    for (size_t xParam = 0; xParam < xParamLimit; xParam++)
    {
        // valid vector lengths are: 0, 1, 2, 4
        if (3 == vlenA || 3 == vlenB || 3 == vlenC)
            continue;

        if (WorkGroup::isActive())
            setWorkGroup(wgSize);

        if (InnerBlocking::isActive())
            setInnerBlocking(1, ibWidth);

        if (VectorLength::isActive())
            setVectorLength(vlenA, vlenB, vlenC);

        if (ExtraParam::isActive())
            setExtraParam(xParam);

        vector< size_t > params;

        if (getParams(params))
        {
            paramSet.insert(params);
            foundParams = true;
        }
    }

    return foundParams;
}

void MatmulMVBase::setParams(const vector<size_t>& params)
{
    size_t index = 0;

    // array precisions
    const size_t precA = params[index++];
    const size_t precB = params[index++];
    const size_t precC = params[index++];
    setPrecision(precA, precB, precC);

    // array vector lengths
    const size_t vlenA = params[index++];
    const size_t vlenB = params[index++];
    const size_t vlenC = params[index++];
    setVectorLength(vlenA, vlenB, vlenC);

    // packed kernels
    const size_t numberKernels = params[index++];
    setPacking(numberKernels);
    switch (params[index++])
    {
        case (0) : setDifferentData(); break;
        case (1) : setSameDataMatrixA(); break;
        case (2) : setSameDataMatrixB(); break;
    }

    // GEMM or pure matrix multiply only
    const size_t GEMM = params[index++];
    setGeneral(GEMM);

    // matrix dimensions
    const size_t M = params[index++];
    const size_t N = params[index++];
    setDimensions(M, N);

    // data layout
    const size_t transposeA = params[index++];
    setDataLayout(transposeA);

    // work group
    const size_t groupSize = params[index++];
    setWorkGroup(groupSize);

    // inner blocking
    const size_t blockWidth = params[index++];
    setInnerBlocking(1, blockWidth);

    // extra parameter
    const size_t extraParam = params[index++];
    setExtraParam(extraParam);
}

string MatmulMVBase::kernelName(void) const
{
    return MatmulBase::kernelName();
}

vector<size_t> MatmulMVBase::globalWorkItems(void) const
{
    vector<size_t> dims;

    if (transposeA())
    {
        dims.push_back(dimensionM() / blockWidth());
    }
    else
    {
        dims.push_back(dimensionM() / effVectorLengthC());
    }

    return dims;
}

vector<size_t> MatmulMVBase::localWorkItems(void) const
{
    vector<size_t> dims;

    dims.push_back(groupSize());

    return dims;
}

size_t MatmulMVBase::numberFlops(void) const
{
    if (general())
        return packing() * dimensionM() * (2 * dimensionN() + 1);
    else
        return packing() * dimensionM() * (2 * dimensionN() - 1);
}

////////////////////////////////////////
// Evergreen::MatmulMV

bool MatmulMV::setArgsInternal(OCLkernel& ckernel,
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
                                       : localSize() * blockWidth();

        switch (_B->precision())
        {
            case (PrecType::UInt32) :
                {
                OCLLocalbuf<uint32_t> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Int32) :
                {
                OCLLocalbuf<int32_t> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Float) :
                {
                OCLLocalbuf<float> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;

            case (PrecType::Double) :
                {
                OCLLocalbuf<double> localTmp( numberElems );
                if (! (ckernel << localTmp).statusOp())
                    return false;
                }
                break;
        }
    }

    // array dimensions
    if (! inlineDim())
    {
        if (! (ckernel << static_cast<int>(dimensionM())).statusOp())
            return false;

        if (! (ckernel << static_cast<int>(dimensionN())).statusOp())
            return false;
    }

    // GEMM alpha and beta
    if (general())
    {
        // alpha
        switch ( maxPrecision(_A->precision(), _B->precision()) )
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
        switch (_C->precision())
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

MatmulMV::MatmulMV(const size_t deviceIndex)
    : MatmulMVBase(),
      ParamInlineDim(getExtraParam()),
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

MatmulMV::~MatmulMV(void)
{
    delete _A;
    delete _B;
    delete _C;
}

void MatmulMV::noCheck(void)
{
    _checkOutputMode = NO_CHECK;
    _checkOutputTouched = true;
}

void MatmulMV::sentinelCheck(void)
{
    _checkOutputMode = SENTINEL_CHECK;
    _checkOutputTouched = true;
}

void MatmulMV::paranoidCheck(const double absDiffLimit)
{
    _checkOutputMode = PARANOID_CHECK;
    _checkOutputTouched = true;
    _paranoidLimit   = absDiffLimit;
}

bool MatmulMV::setArgs(OCLdevice& cdev,
                       OCLkernel& ckernel)
{
    const size_t P = packing();
    const size_t packA = getSameDataMatrixA() ? 1 : P;
    const size_t packB = getSameDataMatrixB() ? 1 : P;

    const size_t M = dimensionM();
    const size_t N = dimensionN();

    const size_t precA = precisionA();
    const size_t precB = precisionB();
    const size_t precC = precisionC();

    const size_t vlenA = vectorLengthA();
    const size_t vlenB = vectorLengthB();
    const size_t vlenC = vectorLengthC();

    cdev.scavenge();

    ArrayBuf *A = NULL, *B = NULL, *C = NULL;

    // allocate arrays
    if (NULL == _A || NULL == _B || NULL == _C ||
        _checkOutputTouched ||
        packingChanged() ||
        generalChanged() ||
        precisionChanged() ||
        dimensionChanged() ||
        layoutChanged() ||
        vectorLengthChanged())
    {
        _checkOutputTouched = true;

        A = transposeA()
            ? new ArrayBuf(cdev, ArrayBuf::READ, packA, M, N, precA, vlenA)
            : new ArrayBuf(cdev, ArrayBuf::READ, packA, N, M, precA, vlenA);

        B = new ArrayBuf(cdev, ArrayBuf::READ, packB, N, 1, precB, vlenB);

        C = new ArrayBuf(cdev,
                         general() ? ArrayBuf::READWRITE : ArrayBuf::WRITE,
                         P,
                         M,
                         1,
                         precC,
                         vlenC,
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
    if ( PrecType::isTypeFP(precA) &&
         PrecType::isTypeFP(precB) )
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
    if ( PrecType::isTypeFP(precC) )
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

bool MatmulMV::setArgs(OCLkernel& ckernel,
                       ArrayBuf* A,
                       ArrayBuf* B,
                       ArrayBuf* C,
                       const double alpha,
                       const double beta)
{
    return setArgsInternal(ckernel, A, B, C, alpha, beta);
}

void MatmulMV::clearArgs(void)
{
    // do not delete array buffer objects, they are owned by the caller
    _A = NULL;
    _B = NULL;
    _C = NULL;

    _alpha = 1;
    _beta = 1;
}

bool MatmulMV::syncInput(void)
{
    // even if A and B do not change, C should be reset
    // to simplify detection of bad output
    return _A->flushToDevice() && _B->flushToDevice() && _C->flushToDevice();
}

bool MatmulMV::syncOutput(void)
{
    return _C->flushFromDevice();
}

bool MatmulMV::checkOutput(void)
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
                                             ? dimensionN() + 1
                                             : dimensionN();

            return sentinelValue == (*histMap.begin()).first;
        }
    }
    else if (PARANOID_CHECK == _checkOutputMode)
    {
        double accumC = 0;

        // reference calculation
        for (size_t p = 0; p < packing(); p++)
        for (size_t j = 0; j < dimensionM(); j++)
        {
            double valueC = 0;

            const size_t pA = getSameDataMatrixA() ? 0 : p;
            const size_t pB = getSameDataMatrixB() ? 0 : p;

            if (transposeA())
            {
                for (size_t l = 0; l < dimensionN(); l++)
                {
                    valueC += _A->getArrayElem(pA, l, j) *
                              _B->getArrayElem(pB, 0, l);
                }
            }
            else
            {
                for (size_t l = 0; l < dimensionN(); l++)
                {
                    valueC += _A->getArrayElem(pA, j, l) *
                              _B->getArrayElem(pB, 0, l);
                }
            }

            if (general())
            {
                valueC = _alpha * valueC
                       + _beta * _C->getHostArrayElem(p, 0, j);
            }

            _C->setHostArrayElem(p, 0, j, valueC);

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

        return true;

        // extra statistical test for floating point
        if ( PrecType::isTypeFP(precisionA()) &&
             PrecType::isTypeFP(precisionB()) &&
             PrecType::isTypeFP(precisionC()) )
        {
            // Elements of A and B are uniform on [0, 1) so mean value of C
            // is predictable.
            // simple matrix-vector multiply: .5 * .5 * N
            // general matrix-vector multiply: alpha * .5 * .5 * N + beta * .5
            const double expectMean = general()
                             ? _alpha * .25f * dimensionN() + _beta * .5f
                             : .25f * dimensionN();

            const double actualMean = accumC / (packing() *
                                                dimensionM());

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

ostream& MatmulMV::print(ostream& os) const
{
    const size_t precA = precisionA();
    const size_t precB = precisionB();
    const size_t precC = precisionC();

    const size_t vlenA = vectorLengthA();
    const size_t vlenB = vectorLengthB();
    const size_t vlenC = vectorLengthC();

    const size_t vnumA = blockWidth() / effVectorLengthA();
    const size_t vnumB = blockWidth() / effVectorLengthB();
    const size_t vnumC = blockWidth() / effVectorLengthC();

    const size_t packIdxDisableA = getSameDataMatrixA() ? 0 : 1;
    const size_t packIdxDisableB = getSameDataMatrixB() ? 0 : 1;

    ////////////////////////////////////////
    // OpenCL double precision extension
    if (PrecType::Double == precA ||
        PrecType::Double == precB ||
        PrecType::Double == precC)
    {
        os << pragma_extension(PrecType::Double, _deviceIndex);
    }

    ////////////////////////////////////////
    // kernel arguments
    FunctionDeclaration kDecl(kernelName());

    const GlobalVar matrixC("matrixC", precC, vlenC, true, kDecl);
    const GlobalVar matrixA("matrixA", precA, vlenA, false, kDecl);
    const GlobalVar matrixB("matrixB", precB, vlenB, false, kDecl);

    const LocalVar localB("localB", precB, vlenB, true, kDecl, 0 != vlenB);

    const PrivateVar M("M", 1, 1, false, kDecl, dimensionM(), inlineDim());
    const PrivateVar N("N", 1, 1, false, kDecl, dimensionN(), inlineDim());

    const PrivateVar alpha("alpha",
                           maxPrecision(precA, precB),
                           1,
                           false,
                           kDecl,
                           general());

    const PrivateVar beta("beta",
                          precC,
                          1,
                          false,
                          kDecl,
                          general());

    // declare kernel
    os << kDecl;

    ////////////////////////////////////////
    // image sampler
    const SamplerVar sampler;
    if (0 == vlenA || 0 == vlenB)
        os << declare(sampler, ImageSampler());

    ////////////////////////////////////////
    // pointers to memory buffers
    const GlobalVar ptrMatrixA("ptrMatrixA", precA, vlenA, false);
    if (0 != vlenA)
        os << declare(ptrMatrixA);
    const GlobalVar ptrMatrixB("ptrMatrixB", precB, vlenB, false);
    if (0 != vlenB)
        os << declare(ptrMatrixB);

    ////////////////////////////////////////
    // accumulate inner product sum
    const size_t accumPrec = maxPrecision(precA, precB, precC);
    const vector< PrivateVar > accum = privateVector(
                                           "accum",
                                           accumPrec,
                                           vlenC,
                                           transposeA() ? vnumC : 1 );
    os << declare(accum);

    ////////////////////////////////////////
    // registers for inner product
    const vector< PrivateVar > valueA = privateVector(
                                            "valueA",
                                            precA,
                                            vlenA,
                                            vnumA );
    const vector< PrivateVar > valueB = privateVector( 
                                            "valueB",
                                            precB,
                                            vlenB,
                                            transposeA() ? 1 : vnumB );
    os << declare(valueA)
       << declare(valueB);

    ////////////////////////////////////////
    // begin packing loop
    const PrivateVar packIdx("packIdx", 1, 1, true, 0, 1 == packing());
    if (1 != packing())
        os << ForLoop(packIdx, packing(), 1);

    // set accumulators to zero
    os << assign(accum, CastValue(ConstantValue(0), accumPrec, vlenC));

    // initialize pointer to memory buffer for matrix A
    if (0 != vlenA)
    {
        // start at left or top edge of matrix A
        if (transposeA())
        {
            // start at top edge of matrix A (height is N, width is M)
            // output for this ID is blockWidth() elements
            os << assign( ptrMatrixA,

                      matrixA

                      // down by packed NxM matrices
                      + packIdxDisableA * (packIdx * (M * N / vlenA))

                      // across by global index
                      + globalCol * vnumA );
        }
        else
        {
            // start at left edge of matrix A (height is M, width is N)
            // output for this ID is effVectorLengthC() elements
            os << assign( ptrMatrixA,

                      matrixA

                      // down by packed MxN matrices
                      + packIdxDisableA * (packIdx * (M * N / vlenA))

                      // down by global index
                      + globalCol * (effVectorLengthC() * N / vlenA) );
        }
    }

    // initialize pointer to memory buffer for matrix B
    if (0 != vlenB)
    {
        // start at top edge of matrix B (height is 1, width is N)
        os << assign( ptrMatrixB,

                  matrixB

                  // down by packed 1xN matrices
                  + packIdxDisableB * (packIdx * (N / vlenB))

                  // across by local index
                  + localCol );
    }

    ////////////////////////////////////////
    // reduction loop
    const PrivateVar outerIdx("outerIdx", 1, 1, true);
    if (transposeA()) // A is transposed, column major
    {
        if (0 == vlenB)
        {
            ////////////////////////////////////////
            // A transposed, B image

            // B is image so iterate directly over elements
            os << ForLoop(outerIdx, N / effVectorLengthB(), 1);

            // read in value of B
            os << assign( valueB[0],
                          ReinterpretValue(
                              ReadImage(
                                  matrixB,
                                  sampler,
                                  outerIdx,
                                  packIdxDisableB * packIdx ),
                              precB,
                              vlenB,
                              PrecType::Double == precB ));

            // iterate over rows of A within the vector length of B
            for (size_t oIdx = 0; oIdx < effVectorLengthB(); oIdx++)
            {
                // read in one block from A
                for (size_t i = 0; i < vnumA; i++)
                {
                    if (0 == vlenA)
                    {
                        // A is image
                        os << assign( valueA[i],
                            ReinterpretValue(
                                ReadImage(
                                    matrixA,
                                    sampler,
                                    globalCol * vnumA + i,
                                    packIdxDisableA * (packIdx * N)
                                        + outerIdx * effVectorLengthB()
                                        + oIdx ),
                                precA,
                                vlenA,
                                PrecType::Double == precA ));
                    }
                    else
                    {
                        // A is memory buffer
                        os << assign( valueA[i], *(ptrMatrixA + i) );
                    }
                }

                // next row of A if memory buffer
                if (0 != vlenA)
                    os << increment( ptrMatrixA, M / vlenA );

                // accumulate
                for (size_t j = 0; j < blockWidth(); j++)
                    os << assignMAD(accum, valueA, valueB, j, oIdx);
            }
        }
        else
        {
            ////////////////////////////////////////
            // A transposed, B memory buffer

            // B is memory buffer so need inner blocking to read into local
            os << ForLoop(outerIdx, N / (groupSize() * effVectorLengthB()), 1);

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
                for (size_t oIdx = 0; oIdx < effVectorLengthB(); oIdx++)
                {
                    // read in one block from A
                    for (size_t i = 0; i < vnumA; i++)
                    {
                        if (0 == vlenA)
                        {
                            // A is image
                            os << assign( valueA[i],
                                ReinterpretValue(
                                    ReadImage(
                                        matrixA,
                                        sampler,
                                        globalCol * vnumA + i,
                                        packIdxDisableA * (packIdx * N)
                                            + outerIdx * ( groupSize()
                                                         * effVectorLengthB() )
                                            + blockIdx * effVectorLengthB()
                                            + oIdx ),
                                    precA,
                                    vlenA,
                                    PrecType::Double == precA ));
                        }
                        else
                        {
                            // A is memory buffer
                            os << assign( valueA[i], *(ptrMatrixA + i) );
                        }
                    }

                    // next row of A if memory buffer
                    if (0 != vlenA)
                        os << increment( ptrMatrixA, M / vlenA );

                    // accumulate
                    for (size_t j = 0; j < blockWidth(); j++)
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
            if (0 == vlenC)
            {
                os << WriteImage(
                          matrixC,
                          switchGlobalCol * vnumC + i,
                          packIdx * N,
                          ReinterpretValue(
                              ConvertValue(
                                  accum[i],
                                  precC,
                                  vlenC,
                                  accumPrec != precC ),
                              PrecType::UInt32,
                              PrecType::vecLength(PrecType::UInt32),
                              PrecType::Double == precC ));
            }
            else
            {
                const GlobalVar ptrMatrixC( matrixC
                                            + packIdx * (M / vlenC)
                                            + switchGlobalCol * vnumC
                                            + i );

                os << assign(
                          GlobalVar(*ptrMatrixC),
                          ConvertValue(
                              general()
                                  ? MADValue(CastValue(alpha, accumPrec, vlenC),
                                             accum[i],
                                             CastValue(beta, accumPrec, vlenC) *
                                             *ptrMatrixC,
                                             useMADFunction())
                                  : accum[i],
                              precC,
                              vlenC,
                              accumPrec != precC ));
            }
        }
    }
    else // A is not transposed, row major
    {
        if (0 == vlenB)
        {
            ////////////////////////////////////////
            // A row major, B image

            // B is image so just iterate directly over blocks
            os << ForLoop(outerIdx, N / blockWidth(), 1);

            // read in one block from B
            for (size_t i = 0; i < vnumB; i++)
            {
                os << assign( valueB[i],
                          ReinterpretValue(
                              ReadImage(
                                  matrixB,
                                  sampler,
                                  outerIdx * vnumB + i,
                                  packIdxDisableB * packIdx ),
                              precB,
                              vlenB,
                              PrecType::Double == precB ));
            }

            // each component of output vector C corresponds to a row of A
            for (size_t oIdx = 0; oIdx < effVectorLengthC(); oIdx++)
            {
                // read in one block from A
                for (size_t i = 0; i < vnumA; i++)
                {
                    if (0 == vlenA)
                    {
                        // A is image
                        os << assign( valueA[i],
                            ReinterpretValue(
                                ReadImage(
                                    matrixA,
                                    sampler,
                                    outerIdx * vnumA + i,
                                    packIdxDisableA * (packIdx * M)
                                        + globalCol * effVectorLengthC()
                                        + oIdx ),
                                precA,
                                vlenA,
                                PrecType::Double == precA ));
                    }
                    else
                    {
                        // A is memory buffer
                        os << assign( valueA[i],
                                      *(ptrMatrixA + oIdx * N / vlenA
                                                   + outerIdx * vnumA
                                                   + i) );
                    }
                }

                // accumulate
                for (size_t l = 0; l < blockWidth(); l++)
                    os << assignMAD(accum, valueA, valueB, oIdx, l);
            }
        }
        else
        {
            ////////////////////////////////////////
            // A row major, B memory buffer

            // B is memory buffer so need inner blocking to read into local
            os << ForLoop(outerIdx, N / (groupSize() * blockWidth()), 1);

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
                for (size_t oIdx = 0; oIdx < effVectorLengthC(); oIdx++)
                {
                    // read in one block from A
                    for (size_t i = 0; i < vnumA; i++)
                    {
                        if (0 == vlenA)
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
                                        packIdxDisableA * (packIdx * M)
                                            + globalCol * effVectorLengthC()
                                            + oIdx ),
                                    precA,
                                    vlenA,
                                    PrecType::Double == precA ));
                        }
                        else
                        {
                            // A is memory buffer
                            os << assign( valueA[i],
                                *(ptrMatrixA + oIdx * N / vlenA
                                             + outerIdx * (groupSize() * vnumA)
                                             + blockIdx * vnumA
                                             + i) );
                        }
                    }

                    // accumulate
                    for (size_t l = 0; l < blockWidth(); l++)
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
        if (0 == vlenC)
        {
            os << WriteImage(
                      matrixC,
                      switchGlobalCol,
                      packIdx * M,
                      ReinterpretValue(
                          ConvertValue(
                              accum[0],
                              precC,
                              vlenC,
                              accumPrec != precC ),
                          PrecType::UInt32,
                          PrecType::vecLength(PrecType::UInt32),
                          PrecType::Double == precC ));
        }
        else
        {
            const GlobalVar ptrMatrixC( matrixC
                                        + packIdx * (M / vlenC)
                                        + switchGlobalCol );

            os << assign(
                      GlobalVar(*ptrMatrixC),
                          ConvertValue(
                              general()
                                  ? MADValue(CastValue(alpha, accumPrec, vlenC),
                                             accum[0],
                                             CastValue(beta, accumPrec, vlenC) *
                                             *ptrMatrixC,
                                             useMADFunction())
                                  : accum[0],
                              precC,
                              vlenC,
                              accumPrec != precC ));
        }
    }

    ////////////////////////////////////////
    // end packing loop
    if (1 != packing())
        os << EndBlock();

    return os << EndBlock();
}

}; // namespace Evergreen

}; // namespace chai_internal
