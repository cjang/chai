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
#include "EvergreenMatmulMM.hpp"
#include "Logger.hpp"

using namespace std;

namespace chai_internal {

namespace Evergreen {

////////////////////////////////////////
// Evergreen::MatmulMMBase

MatmulMMBase::MatmulMMBase(void)
    : MatmulBase() { }

MatmulMMBase::~MatmulMMBase(void) { }

string MatmulMMBase::assignMAD(
                         const vector< vector< PrivateVar > >& accum,
                         const vector< vector< PrivateVar > >& valueA,
                         const vector< vector< PrivateVar > >& valueB,
                         const size_t j,        // output row
                         const size_t k,        // output column
                         const size_t l ) const // inner product component
{
    const size_t vlenA = valueA[0][0].vectorLength();
    const size_t vlenB = valueB[0][0].vectorLength();
    const size_t vlenC = accum[0][0].vectorLength();

    if (transposeA())
    {
        const size_t offset = (l % vlenA) + vlenA * (j / vlenA);

        if (transposeB())
            // At Bt
            return assign( accum[k / vlenC][j][k % vlenC],
                           MADValue(
                               valueA[l / vlenA][offset][j % vlenA],
                               valueB[l / vlenB][k][l % vlenB],
                               accum[k / vlenC][j][k % vlenC] ));
        else
            // At B
            return assign( accum[k / vlenC][j][k % vlenC],
                           MADValue(
                               valueA[l / vlenA][offset][j % vlenA],
                               valueB[k / vlenB][l][k % vlenB],
                               accum[k / vlenC][j][k % vlenC] ));
    }
    else
    {
        if (transposeB())
            // A Bt
            return assign( accum[k / vlenC][j][k % vlenC],
                           MADValue(
                               valueA[l / vlenA][j][l % vlenA],
                               valueB[l / vlenB][k][l % vlenB],
                               accum[k / vlenC][j][k % vlenC] ));
        else
            // A B
            return assign( accum[k / vlenC][j][k % vlenC],
                           MADValue(
                               valueA[l / vlenA][j][l % vlenA],
                               valueB[k / vlenB][l][k % vlenB],
                               accum[k / vlenC][j][k % vlenC] ));
    }
}

ostream& MatmulMMBase::assignMAD(
                           ostream& os,
                           const size_t loopOrder,
                           const vector< vector< PrivateVar > >& accum,
                           const vector< vector< PrivateVar > >& valueA,
                           const vector< vector< PrivateVar > >& valueB ) const
{
    switch (loopOrder)
    {
        case (0) : // (j, k, l)

            // output row
            for (size_t j = 0; j < blockHeight(); j++)

            // output column
            for (size_t k = 0; k < blockWidth(); k++)

            // values component
            for (size_t l = 0; l < blockWidth(); l++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (1) : // (k, j, l)

            // output column
            for (size_t k = 0; k < blockWidth(); k++)

            // output row
            for (size_t j = 0; j < blockHeight(); j++)

            // values component
            for (size_t l = 0; l < blockWidth(); l++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (2) : // (l, j, k)

            // values component
            for (size_t l = 0; l < blockWidth(); l++)

            // output row
            for (size_t j = 0; j < blockHeight(); j++)

            // output column
            for (size_t k = 0; k < blockWidth(); k++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (3) : // (j, l, k)

            // output row
            for (size_t j = 0; j < blockHeight(); j++)

            // values component
            for (size_t l = 0; l < blockWidth(); l++)

            // output column
            for (size_t k = 0; k < blockWidth(); k++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (4) : // (k, l, j)

            // output column
            for (size_t k = 0; k < blockWidth(); k++)

            // values component
            for (size_t l = 0; l < blockWidth(); l++)

            // output row
            for (size_t j = 0; j < blockHeight(); j++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (5) : // (l, k, j)

            // values component
            for (size_t l = 0; l < blockWidth(); l++)

            // output column
            for (size_t k = 0; k < blockWidth(); k++)

            // output row
            for (size_t j = 0; j < blockHeight(); j++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;
    }

    return os;
}

string MatmulMMBase::kernelForename(void) const
{
    return "matmul";
}

bool MatmulMMBase::validParams(void) const
{
    return
        // array types of A, B, C
        validArrayType(precisionA(), vectorLengthA()) &&
        validArrayType(precisionB(), vectorLengthB()) &&
        validArrayType(precisionC(), vectorLengthC()) &&

        // if GEMM, then C must be read-write (can not be an image)
        !(general() && 0 == vectorLengthC()) &&

        // packed kernels
        packing() > 0 &&

        // check for blocking compatible with matrix dimensions
        0 == dimensionM() % (groupHeight() * blockHeight()) &&
        0 == dimensionN() % (groupWidth() * blockWidth()) &&
        0 == dimensionK() % (groupHeight() * blockWidth()) &&
        groupHeight() * blockHeight() <= dimensionM() &&
        groupWidth() * blockWidth() <= dimensionN() &&
        groupHeight() * blockWidth() <= dimensionK() &&

        // if A is transposed, then inner blocking height
        // must be even number of vector elements
        ( transposeA()
              ? (0 == blockHeight()
                      % ( 0 == vectorLengthA()
                          ? (sizeof(float) == precisionA() ? 4 : 2)
                          : vectorLengthA() ))
              : true ) &&


        // extra parameter
        extraParam() < totalVariations();
}

bool MatmulMMBase::getParams(vector<size_t>& params) const
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
        params.push_back(dimensionK());

        // data layout
        params.push_back(transposeA());
        params.push_back(transposeB());

        // work group
        params.push_back(groupHeight());
        params.push_back(groupWidth());

        // inner blocking
        params.push_back(blockHeight());
        params.push_back(blockWidth());

        // extra parameter
        params.push_back(extraParam());
    }

    return rc;
}

string MatmulMMBase::exogenousKey(void) const
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
       << (transposeB() ? "T" : "N")
       << "_" << dimensionM()
       << "_" << dimensionN()
       << "_" << dimensionK();

    return ss.str();
}

void MatmulMMBase::toggleParamMarks(void)
{
    MatmulBase::toggleParamMarks();
}

bool MatmulMMBase::getParams(set< vector<size_t> >& paramSet)
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
    //             loop order (extra parameter)
    //             global ID (extra parameter)

    const bool exogenousOk =
        (packing() > 0) &&
        (sizeof(float) == precisionA() || sizeof(double) == precisionA()) &&
        (sizeof(float) == precisionB() || sizeof(double) == precisionB()) &&
        (sizeof(float) == precisionC() || sizeof(double) == precisionC()) &&
        (0 != dimensionM()) &&
        (0 != dimensionN()) &&
        (0 != dimensionK());

    if (! exogenousOk)
        return false;

    const size_t wgStart = WorkGroup::isActive() ? 4 : 0;
    const size_t wgLimit = WorkGroup::isActive() ? 16 : 0;
    const size_t ibHeightStart = InnerBlocking::isActive() ? 2 : 0;
    const size_t ibHeightLimit = InnerBlocking::isActive() ? 12 : 0;
    const size_t ibWidthStart = InnerBlocking::isActive() ? 2 : 0;
    const size_t ibWidthLimit = InnerBlocking::isActive() ? 8 : 0;
    const size_t vlenStart = VectorLength::isActive() ? 0 : 0;
    const size_t vlenLimit = VectorLength::isActive() ? 4 : 0;
    const size_t xParamLimit = ExtraParam::isActive() ? totalVariations() : 1;

    bool foundParams = false;

    // work group
    for (size_t wgSize = wgStart; wgSize <= wgLimit; wgSize++)

    // inner blocking
    for (size_t ibHeight = ibHeightStart; ibHeight <= ibHeightLimit; ibHeight++)
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
            setInnerBlocking(ibHeight, ibWidth);

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

void MatmulMMBase::setParams(const vector<size_t>& params)
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
    const size_t K = params[index++];
    setDimensions(M, N, K);

    // data layout
    const size_t transposeA = params[index++];
    const size_t transposeB = params[index++];
    setDataLayout(transposeA, transposeB);

    // work group
    const size_t groupHeight = params[index++];
    const size_t groupWidth = params[index++];
    setWorkGroup(groupHeight, groupWidth);

    // inner blocking
    const size_t blockHeight = params[index++];
    const size_t blockWidth = params[index++];
    setInnerBlocking(blockHeight, blockWidth);

    // extra parameter
    const size_t extraParam = params[index++];
    setExtraParam(extraParam);
}

string MatmulMMBase::kernelName(void) const
{
    return MatmulBase::kernelName();
}

vector<size_t> MatmulMMBase::globalWorkItems(void) const
{
    vector<size_t> dims;
    dims.push_back(dimensionN() / blockWidth());
    dims.push_back(dimensionM() / blockHeight());
    return dims;
}

vector<size_t> MatmulMMBase::localWorkItems(void) const
{
    vector<size_t> dims;
    dims.push_back(groupWidth());
    dims.push_back(groupHeight());
    return dims;
}

size_t MatmulMMBase::numberFlops(void) const
{
    if (general())
        return packing() * 2 * dimensionM()*dimensionN() * (dimensionK() + 1);
    else
        return packing() * dimensionM()*dimensionN() * (2 * dimensionK() - 1);
}

////////////////////////////////////////
// Evergreen::MatmulMM

bool MatmulMM::setArgsInternal(OCLkernel& ckernel,
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
                                   blockWidth() *
                                   blockHeight();

        if (_A->isSinglePrecision())
        {
            OCLLocalbuf<float> localTmp( numberElems );
            if (! (ckernel << localTmp).statusOp())
                return false;
        }

        if (_A->isDoublePrecision())
        {
            OCLLocalbuf<double> localTmp( numberElems );
            if (! (ckernel << localTmp).statusOp())
                return false;
        }
    }

    // set local memory for array B
    if (_B->isMembuf())
    {
        const size_t numberElems = localSize() *
                                   groupSize() *
                                   blockWidth() *
                                   blockWidth();

        if (_B->isSinglePrecision())
        {
            OCLLocalbuf<float> localTmp( numberElems );
            if (! (ckernel << localTmp).statusOp())
                return false;
        }

        if (_B->isDoublePrecision())
        {
            OCLLocalbuf<double> localTmp( numberElems );
            if (! (ckernel << localTmp).statusOp())
                return false;
        }
    }

    // array dimensions
    if (! inlineDim())
    {
        if (! (ckernel << static_cast<int>(dimensionM())).statusOp())
            return false;

        if (! (ckernel << static_cast<int>(dimensionN())).statusOp())
            return false;

        if (! (ckernel << static_cast<int>(dimensionK())).statusOp())
            return false;
    }

    // GEMM alpha and beta
    if (general())
    {
        // alpha
        if (_A->isSinglePrecision() && _B->isSinglePrecision())
        {
            const float alpha32 = alpha;

            if (! (ckernel << alpha32).statusOp())
                return false;
        }
        else
        {
            const double alpha64 = alpha;

            if (! (ckernel << alpha64).statusOp())
                return false;
        }

        // beta
        if (_C->isSinglePrecision())
        {
            const float beta32 = beta;

            if (! (ckernel << beta32).statusOp())
                return false;
        }
        else
        {
            const double beta64 = beta;

            if (! (ckernel << beta64).statusOp())
                return false;
        }
    }

    return true;
}

MatmulMM::MatmulMM(const size_t deviceIndex)
    : MatmulMMBase(),
      ParamInlineDim(getExtraParam()),
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

MatmulMM::~MatmulMM(void)
{
    delete _A;
    delete _B;
    delete _C;
}

void MatmulMM::noCheck(void)
{
    _checkOutputMode = NO_CHECK;
    _checkOutputTouched = true;
}

void MatmulMM::sentinelCheck(void)
{
    _checkOutputMode = SENTINEL_CHECK;
    _checkOutputTouched = true;
}

void MatmulMM::paranoidCheck(const double absDiffLimit)
{
    _checkOutputMode = PARANOID_CHECK;
    _checkOutputTouched = true;
    _paranoidLimit   = absDiffLimit;
}

bool MatmulMM::setArgs(OCLdevice& cdev,
                       OCLkernel& ckernel)
{
    const size_t P = packing();
    const size_t packA = getSameDataMatrixA() ? 1 : P;
    const size_t packB = getSameDataMatrixB() ? 1 : P;

    const size_t M = dimensionM();
    const size_t N = dimensionN();
    const size_t K = dimensionK();

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
            ? new ArrayBuf(cdev, ArrayBuf::READ, packA, M, K, precA, vlenA)
            : new ArrayBuf(cdev, ArrayBuf::READ, packA, K, M, precA, vlenA);

        B = transposeB()
            ? new ArrayBuf(cdev, ArrayBuf::READ, packB, K, N, precB, vlenB)
            : new ArrayBuf(cdev, ArrayBuf::READ, packB, N, K, precB, vlenB);

        C = new ArrayBuf(cdev,
                         general() ? ArrayBuf::READWRITE : ArrayBuf::WRITE,
                         P,
                         N,
                         M,
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

    const double alpha = PARANOID_CHECK == _checkOutputMode
                             ? posrand<double>()
                             : 1;

    const double beta = PARANOID_CHECK == _checkOutputMode
                            ? posrand<double>()
                            : 1;

    return setArgsInternal(ckernel, A, B, C, alpha, beta);
}

bool MatmulMM::setArgs(OCLkernel& ckernel,
                       ArrayBuf* A,
                       ArrayBuf* B,
                       ArrayBuf* C,
                       const double alpha,
                       const double beta)
{
    return setArgsInternal(ckernel, A, B, C, alpha, beta);
}

void MatmulMM::clearArgs(void)
{
    // do not delete array buffer objects, they are owned by the caller
    _A = NULL;
    _B = NULL;
    _C = NULL;

    _alpha = 1;
    _beta = 1;
}

bool MatmulMM::syncInput(void)
{
    // even if A and B do not change, C should be reset
    // to simplify detection of bad output
    return _A->flushToDevice() && _B->flushToDevice() && _C->flushToDevice();
}

bool MatmulMM::syncOutput(void)
{
    return _C->flushFromDevice();
}

bool MatmulMM::checkOutput(void)
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
                                             ? dimensionK() + 1
                                             : dimensionK();

            return sentinelValue == (*histMap.begin()).first;
        }
    }
    else if (PARANOID_CHECK == _checkOutputMode)
    {
        double accumC = 0;

        // reference calculation
        for (size_t p = 0; p < packing(); p++)
        for (size_t i = 0; i < dimensionM(); i++)
        for (size_t j = 0; j < dimensionN(); j++)
        {
            double valueC = 0;

            const size_t pA = getSameDataMatrixA() ? 0 : p;
            const size_t pB = getSameDataMatrixB() ? 0 : p;

            for (size_t k = 0; k < dimensionK(); k++)
            {
                valueC += ( transposeA()
                                ? _A->getArrayElem(pA, k, i)
                                : _A->getArrayElem(pA, i, k) )
                          *
                          ( transposeB()
                                ? _B->getArrayElem(pB, j, k)
                                : _B->getArrayElem(pB, k, j) );
            }

            if (general())
            {
                valueC = _alpha * valueC
                       + _beta * _C->getHostArrayElem(p, i, j);
            }

            _C->setHostArrayElem(p, i, j, valueC);

            accumC += valueC;
        }

        // aggregate L1 error in calculation
        const double absdiffError = _C->absdiffHostArray();

        // Elements of A and B are uniform on [0, 1) so mean value of C
        // is predictable.
        // simple matrix multiply: .5 * .5 * K
        // general matrix multiply: alpha * .5 * .5 * K + beta * .5
        const double expectMean = general()
                         ? _alpha * .25f * dimensionK() + _beta * .5f
                         : .25f * dimensionK();

        const double actualMean = accumC / (packing() *
                                            dimensionM() *
                                            dimensionN());

        // percentage error from statistically expected value, this detects
        // uninitialized buffer failures as they are usually filled with zeros
        const double mape = fabs((actualMean - expectMean) / expectMean);

        // MAPE of 50 basis points seems like a lot but for small matrices
        // is reasonable, for example, 10 basis points is often exceeded
        if ( absdiffError < _paranoidLimit && mape < 0.5f )
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

    return false;
}

ostream& MatmulMM::print(ostream& os) const
{
    const size_t precA = precisionA();
    const size_t precB = precisionB();
    const size_t precC = precisionC();

    const size_t vlenA = vectorLengthA();
    const size_t vlenB = vectorLengthB();
    const size_t vlenC = vectorLengthC();

    const size_t vnumA = blockWidth() / (0 == vlenA
                                             ? (sizeof(float) == precA ? 4 : 2)
                                             : vlenA);
    const size_t vnumB = blockWidth() / (0 == vlenB
                                             ? (sizeof(float) == precB ? 4 : 2)
                                             : vlenB);
    const size_t vnumC = blockWidth() / (0 == vlenC
                                             ? (sizeof(float) == precC ? 4 : 2)
                                             : vlenC);

    ////////////////////////////////////////
    // OpenCL double precision extension
    os << pragma_extension( sizeof(double) == precA ||
                            sizeof(double) == precB ||
                            sizeof(double) == precC
                                ? sizeof(double) : sizeof(float),
                            _deviceIndex );

    ////////////////////////////////////////
    // kernel arguments
    FunctionDeclaration kDecl(kernelName());

    const GlobalVar matrixC("matrixC", precC, vlenC, true, kDecl);
    const GlobalVar matrixA("matrixA", precA, vlenA, false, kDecl);
    const GlobalVar matrixB("matrixB", precB, vlenB, false, kDecl);

    const LocalVar localA("localA", precA, vlenA, true, kDecl, 0 != vlenA);
    const LocalVar localB("localB", precB, vlenB, true, kDecl, 0 != vlenB);

    const PrivateVar M("M", 1, 1, false, kDecl, dimensionM(), inlineDim());
    const PrivateVar N("N", 1, 1, false, kDecl, dimensionN(), inlineDim());
    const PrivateVar K("K", 1, 1, false, kDecl, dimensionK(), inlineDim());

    const PrivateVar alpha("alpha",
                           sizeof(double) == precA || sizeof(double) == precB
                               ? sizeof(double)
                               : sizeof(float),
                           1,
                           false,
                           kDecl,
                           general());

    const PrivateVar beta("beta",
                          sizeof(double) == precC
                              ? sizeof(double)
                              : sizeof(float),
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
    {
        os << declare(sampler, ImageSampler());
    }

    ////////////////////////////////////////
    // pointers to memory buffers
    const GlobalVar ptrMatrixA("ptrMatrixA", precA, vlenA, false);
    const LocalVar ptrLocalA("ptrLocalA", precA, vlenA, false);
    if (0 != vlenA)
    {
        os << declare(ptrMatrixA);
        os << declare(ptrLocalA);
    }
    const GlobalVar ptrMatrixB("ptrMatrixB", precB, vlenB, false);
    const LocalVar ptrLocalB("ptrLocalB", precB, vlenB, false);
    if (0 != vlenB)
    {
        os << declare(ptrMatrixB);
        os << declare(ptrLocalB);
    }

    ////////////////////////////////////////
    // accumulate inner product sum
    const vector< vector< PrivateVar > > accum = privateVector(
                                                     "accum",
                                                     precC,
                                                     vlenC,
                                                     blockHeight(),
                                                     vnumC);
    os << declare(accum);

    ////////////////////////////////////////
    // registers for inner product
    const vector< vector< PrivateVar > > valueA = privateVector(
                                                      "valueA",
                                                      precA,
                                                      vlenA,
                                                      blockHeight(),
                                                      vnumA);
    const vector< vector< PrivateVar > > valueB = privateVector(
                                                      "valueB",
                                                      precB,
                                                      vlenB,
                                                      blockWidth(),
                                                      vnumB);
    os << declare(valueA)
       << declare(valueB);

    ////////////////////////////////////////
    // begin packing loop
    const PrivateVar packIdx("packIdx", 1, 1, true, 0, 1 == packing());
    if (1 != packing())
        os << ForLoop(packIdx, packing(), 1);

    // set accumulators to zero
    os << assign(accum, CastValue(ConstantValue(0), precC, vlenC));

    // initialize pointer to memory buffer for matrix A
    if (0 != vlenA)
    {
        const size_t packIdxDisable = getSameDataMatrixA() ? 0 : 1;

        // start at left or top edge of matrix A
        if (transposeA())
        {
            // start at top edge of matrix A (height is K, width is M)
            // (must transpose quads into local memory)
            os << assign( ptrMatrixA,

                      matrixA

                      // down by packed KxM matrices
                      + packIdxDisable * (packIdx * (M * K / vlenA))

                      // across by global index
                      + globalRow * (blockHeight() / vlenA)

                      // down by local index
                      + localCol * (M * (blockWidth() / vlenA)) );
        }
        else
        {
            // start at left edge of matrix A (height is M, width is K)
            os << assign( ptrMatrixA,

                      matrixA

                      // down by packed MxK matrices
                      + packIdxDisable * (packIdx * (M * K / vlenA))

                      // down by work group
                      + groupRow *
                        ( 0 == (groupSize() * blockHeight()) % vlenA
                              ? K * (groupSize() * blockHeight() / vlenA)
                              : (groupSize() * blockHeight()) * K / vlenA )

                      // down by local index
                      + localRow * (K / vlenA)

                      // across by local index
                      + localCol );
        }
    }

    // initialize pointer to memory buffer for matrix B
    if (0 != vlenB)
    {
        const size_t packIdxDisable = getSameDataMatrixB() ? 0 : 1;

        // start at left or top edge of matrix B
        if (transposeB())
        {
            // start at left edge of matrix B (height is N, width is K)
            os << assign( ptrMatrixB,

                      matrixB

                      // down by packed NxK matrices
                      + packIdxDisable * (packIdx * (N * K / vlenB))

                      // down by work group
                      + groupCol * (K * (groupSize() * blockWidth() / vlenB))

                      // down by local index
                      + localRow * (K / vlenB)

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
                      + packIdxDisable * (packIdx * (N * K / vlenB))

                      // across by global index
                      + globalCol * (blockWidth() / vlenB)

                      // down by local index
                      + localRow * (N * (blockWidth() / vlenB)) );
        }
    }

    ////////////////////////////////////////
    // begin outer loop over blocks
    const PrivateVar outerIdx("outerIdx", 1, 1, true);
    if (0 != vlenA || 0 != vlenB)
    {
        os << ForLoop(outerIdx, K / (groupSize() * blockWidth()), 1);
    }

    // copy from global memory buffer for matrix A to local memory
    if (0 != vlenA)
    {
        for (size_t i = 0; i < blockHeight(); i++)
        for (size_t j = 0; j < vnumA; j++)
        {
            if (transposeA())
            {
                const size_t blockNum = i / vlenA;
                const size_t blockIdx = i % vlenA;

                os << assign( LocalVar(

                *(localA + (localRow * (blockHeight() * localSize()) +
                            localCol +
                            i * localSize()) * vnumA
                         + j) ),

                *(ptrMatrixA + blockNum
                             + blockIdx * M / vlenA
                             + j * M) );
            }
            else
            {
                os << assign( LocalVar(

                *(localA + localRow * (localSize() * vnumA)
                         + i * groupSize() * localSize() * vnumA
                         + localCol
                         + j * groupSize()) ),

                *(ptrMatrixA + (i * groupSize()) * K / vlenA
                             + j * groupSize()) );
            }
        }
    }

    // copy from global memory buffer for matrix B to local memory
    if (0 != vlenB)
    {
        for (size_t i = 0; i < blockWidth(); i++)
        for (size_t j = 0; j < vnumB; j++)
        {
            if (transposeB())
            {
                os << assign( LocalVar(

                *(localB + localRow * (localSize() * vnumB)
                         + i * groupSize() * localSize() * vnumB
                         + localCol
                         + j * groupSize()) ),

                *(ptrMatrixB + (i * groupSize()) * K / vlenB
                             + j * groupSize()) );
            }
            else
            {
                os << assign( LocalVar(

                *(localB + (localCol * (blockWidth() * localSize()) +
                            localRow) * vnumB
                         + i * localSize() * vnumB
                         + j) ),

                *(ptrMatrixB + i * N / vlenB + j) );
            }
        }
    }

    // need barrier if using local memory
    if (0 != vlenA || 0 != vlenB)
    {
        os << LocalBarrier();
    }

    // update global memory buffer pointer for matrix A
    if (0 != vlenA)
    {
        if (transposeA())
        {
            os << increment( ptrMatrixA, M * (groupSize() * vnumA) );
        }
        else
        {
            os << increment( ptrMatrixA, groupSize() * vnumA );
        }
    }

    // update global memory buffer pointer for matrix B
    if (0 != vlenB)
    {
        if (transposeB())
        {
            os << increment( ptrMatrixB, groupSize() * vnumB );
        }
        else
        {
            os << increment( ptrMatrixB,
                             N * (groupSize() * vnumB) );
        }
    }

    // initialize inner product pointer when using memory buffer for matrix A
    if (0 != vlenA)
    {
        os << assign( ptrLocalA,
                      localA + localRow * (localSize() *
                                           blockHeight() *
                                           vnumA) );
    }

    // initialize inner product pointer when using memory buffer for matrix B
    if (0 != vlenB)
    {
        os << assign( ptrLocalB,
                      localB + localCol * (localSize() *
                                           blockWidth() *
                                           vnumB) );
    }

    ////////////////////////////////////////
    // one loop for A and B both images
    const PrivateVar kIdx("kIdx", 1, 1, true);
    if (0 == vlenA && 0 == vlenB)
    {
        os << ForLoop(kIdx, K / blockWidth(), 1);
    }

    ////////////////////////////////////////
    // begin inner product accumulation
    const PrivateVar innerIdx("innerIdx", 1, 1, true);
    if (0 != vlenA || 0 != vlenB)
    {
        os << ForLoop(innerIdx, groupSize(), 1);
    }

    // read in values of matrix A
    if (0 == vlenA)
    {
        const size_t packIdxDisable = getSameDataMatrixA() ? 0 : 1;

        // image
        for (size_t i = 0; i < blockHeight(); i++)
        for (size_t j = 0; j < vnumA; j++)
        {
            const size_t veclength = sizeof(float) == precA ? 4 : 2;

            const PrivateVar
                idx( 0 == vlenB
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
                            globalRow * (blockHeight() / veclength) + blockNum,
                            packIdxDisable * (packIdx * K)
                                + idx * blockWidth()
                                + (j * veclength + blockIdx) ),
                        precA,
                        vlenA,
                        4 != veclength ));
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
                            packIdxDisable * (packIdx * M)
                                + globalRow * blockHeight()
                                + i ),
                        precA,
                        vlenA,
                        4 != veclength ));
            }
        }
    }
    else
    {
        // memory buffer
        for (size_t i = 0; i < blockHeight(); i++)
        for (size_t j = 0; j < vnumA; j++)
        {
            os << assign( valueA[j][i],
                          *(ptrLocalA + i * localSize() * vnumA + j) );
        }

        os << increment( ptrLocalA, vnumA );
    }

    // read in values of matrix B
    if (0 == vlenB)
    {
        const size_t packIdxDisable = getSameDataMatrixB() ? 0 : 1;

        // image
        for (size_t i = 0; i < blockWidth(); i++)
        for (size_t j = 0; j < vnumB; j++)
        {
            const size_t veclength = sizeof(float) == precB ? 4 : 2;

            const PrivateVar
                idx( 0 == vlenA
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
                            packIdxDisable * (packIdx * N)
                                + globalCol * blockWidth()
                                + i ),
                        precB,
                        vlenB,
                        4 != veclength ));
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
                            packIdxDisable * (packIdx * K)
                                + idx * blockWidth()
                                + i ),
                        precB,
                        vlenB,
                        4 != veclength ));
            }
        }
    }
    else
    {
        // memory buffer
        for (size_t i = 0; i < blockWidth(); i++)
        for (size_t j = 0; j < vnumB; j++)
        {
            os << assign( valueB[j][i],
                          *(ptrLocalB + i * localSize() * vnumB + j) );
        }

        os << increment( ptrLocalB, vnumB );
    }

    // assignMAD goes here
    assignMAD(os, loopOrder(), accum, valueA, valueB);

    if (0 == vlenA && 0 == vlenB)
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

    if (0 == vlenC)
    {
        const size_t veclength = sizeof(float) == precC ? 4 : 2;

        // image
        for (size_t i = 0; i < blockHeight(); i++)
        for (size_t j = 0; j < vnumC; j++)
        {
            os << WriteImage(matrixC,
                             switchGlobalCol * vnumC + j,
                             packIdx * M
                             + switchGlobalRow * blockHeight()
                             + i,
                             ReinterpretValue(accum[j][i],
                                              0,
                                              4,
                                              4 != veclength) );
        }
    }
    else
    {
        // memory buffer
        const GlobalVar ptrMatrixC(
                            matrixC
                            + packIdx * (M * N / vlenC)
                            + switchGlobalRow * (blockHeight() * N / vlenC)
                            + switchGlobalCol * (blockWidth() / vlenC) );

        for (size_t i = 0; i < blockHeight(); i++)
        for (size_t j = 0; j < vnumC; j++)
        {
            os << assign(
                      GlobalVar(*(ptrMatrixC + i * N / vlenC + j)),
                      general()
                          ? MADValue(CastValue(alpha, precC, vlenC),
                                     accum[j][i],
                                     CastValue(beta, precC, vlenC) *
                                     *(ptrMatrixC + i * N / vlenC + j))
                          : accum[j][i] );
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
