// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <map>
#include <math.h>
#include <sstream>

#include "CodeControlFlow.hpp"
#include "CodeFunction.hpp"
#include "CodeImageSample.hpp"
#include "CodeStatements.hpp"
#include "CodeType.hpp"
#include "CodeValues.hpp"
#include "EvergreenMatmulMM.hpp"
#include "Logger.hpp"
#include "PrecType.hpp"
#include "UtilFuns.hpp"

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
    const size_t vlenA = valueA[0][0].vecLength();
    const size_t vlenB = valueB[0][0].vecLength();
    const size_t vlenC = accum[0][0].vecLength();

    if (transposeA())
    {
        const size_t offset = (l % vlenA) + vlenA * (j / vlenA);

        if (transposeB())
            // At Bt
            return assign( accum[k / vlenC][j][k % vlenC],
                           MADValue(
                               valueA[l / vlenA][offset][j % vlenA],
                               valueB[l / vlenB][k][l % vlenB],
                               accum[k / vlenC][j][k % vlenC],
                               useMADFunction() ));
        else
            // At B
            return assign( accum[k / vlenC][j][k % vlenC],
                           MADValue(
                               valueA[l / vlenA][offset][j % vlenA],
                               valueB[k / vlenB][l][k % vlenB],
                               accum[k / vlenC][j][k % vlenC],
                               useMADFunction() ));
    }
    else
    {
        if (transposeB())
            // A Bt
            return assign( accum[k / vlenC][j][k % vlenC],
                           MADValue(
                               valueA[l / vlenA][j][l % vlenA],
                               valueB[l / vlenB][k][l % vlenB],
                               accum[k / vlenC][j][k % vlenC],
                               useMADFunction() ));
        else
            // A B
            return assign( accum[k / vlenC][j][k % vlenC],
                           MADValue(
                               valueA[l / vlenA][j][l % vlenA],
                               valueB[k / vlenB][l][k % vlenB],
                               accum[k / vlenC][j][k % vlenC],
                               useMADFunction() ));
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
            for (size_t j = 0; j < blockH(); j++)

            // output column
            for (size_t k = 0; k < blockW(); k++)

            // values component
            for (size_t l = 0; l < blockW(); l++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (1) : // (k, j, l)

            // output column
            for (size_t k = 0; k < blockW(); k++)

            // output row
            for (size_t j = 0; j < blockH(); j++)

            // values component
            for (size_t l = 0; l < blockW(); l++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (2) : // (l, j, k)

            // values component
            for (size_t l = 0; l < blockW(); l++)

            // output row
            for (size_t j = 0; j < blockH(); j++)

            // output column
            for (size_t k = 0; k < blockW(); k++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (3) : // (j, l, k)

            // output row
            for (size_t j = 0; j < blockH(); j++)

            // values component
            for (size_t l = 0; l < blockW(); l++)

            // output column
            for (size_t k = 0; k < blockW(); k++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (4) : // (k, l, j)

            // output column
            for (size_t k = 0; k < blockW(); k++)

            // values component
            for (size_t l = 0; l < blockW(); l++)

            // output row
            for (size_t j = 0; j < blockH(); j++)

                os << assignMAD(accum, valueA, valueB, j, k, l);

            break;

        case (5) : // (l, k, j)

            // values component
            for (size_t l = 0; l < blockW(); l++)

            // output column
            for (size_t k = 0; k < blockW(); k++)

            // output row
            for (size_t j = 0; j < blockH(); j++)

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
        validArrayType(precA(), vecLengthA()) &&
        validArrayType(precB(), vecLengthB()) &&
        validArrayType(precC(), vecLengthC()) &&

        // if GEMM, then C must be read-write (can not be an image)
        !(general() && 0 == vecLengthC()) &&

        // packed kernels
        batching() > 0 &&

        // check for blocking compatible with matrix dimensions
        0 == M() % (groupH() * blockH()) &&
        0 == N() % (groupW() * blockW()) &&
        0 == K() % (groupH() * blockW()) &&
        groupH() * blockH() <= M() &&
        groupW() * blockW() <= N() &&
        groupH() * blockW() <= K() &&

        // if A is transposed, then inner blocking height
        // must be even number of vector elements
        ( transposeA()
              ? (0 == blockH() % effVecLengthA())
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
        params.push_back(precA());
        params.push_back(precB());
        params.push_back(precC());

        // array vector lengths
        params.push_back(vecLengthA());
        params.push_back(vecLengthB());
        params.push_back(vecLengthC());

        // packed kernels
        params.push_back(batching());
        if (getDifferentData()) params.push_back(0);
        else if (getSameDataMatrixA()) params.push_back(1);
        else if (getSameDataMatrixB()) params.push_back(2);

        // GEMM or pure matrix multiply only
        params.push_back(general());

        // matrix dimensions
        params.push_back(M());
        params.push_back(N());
        params.push_back(K());

        // data layout
        params.push_back(transposeA());
        params.push_back(transposeB());

        // work group
        params.push_back(groupH());
        params.push_back(groupW());

        // inner blocking
        params.push_back(blockH());
        params.push_back(blockW());

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
       << batching()
       << (getSameDataMatrixA() ? "A" : "N")
       << (getSameDataMatrixB() ? "B" : "N")
       << (general() ? "G" : "N")
       << precA()
       << precB()
       << precC()
       << (transposeA() ? "T" : "N")
       << (transposeB() ? "T" : "N")
       << "_" << M()
       << "_" << N()
       << "_" << K();

    return ss.str();
}

void MatmulMMBase::toggleParamMarks(void)
{
    MatmulBase::toggleParamMarks();
}

bool MatmulMMBase::getParams(set< vector<size_t> >& paramSet)
{
    // exogenous: batching (default is 1: no batching)
    //            general (default is false: pure matrix multiply)
    //            precision (default is -1, must be set)
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
        (batching() > 0) &&
        PrecType::validCode(precA()) &&
        PrecType::validCode(precB()) &&
        PrecType::validCode(precC()) &&
        (0 != M()) &&
        (0 != N()) &&
        (0 != K());

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

    // batched kernels
    const size_t numberKernels = params[index++];
    setBatching(numberKernels);
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
    const size_t groupH = params[index++];
    const size_t groupW = params[index++];
    setWorkGroup(groupH, groupW);

    // inner blocking
    const size_t blockH = params[index++];
    const size_t blockW = params[index++];
    setInnerBlocking(blockH, blockW);

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
    dims.push_back(N() / blockW());
    dims.push_back(M() / blockH());
    return dims;
}

vector<size_t> MatmulMMBase::localWorkItems(void) const
{
    vector<size_t> dims;
    dims.push_back(groupW());
    dims.push_back(groupH());
    return dims;
}

size_t MatmulMMBase::numberFlops(void) const
{
    if (general())
        return batching() * 2 * M() * N() * (K() + 1);
    else
        return batching() * M() * N() * (2 * K() - 1);
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
                                   blockW() *
                                   blockH();

        switch (_A->prec())
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

ostream& MatmulMM::print(ostream& os) const
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

}; // namespace Evergreen

}; // namespace chai_internal
