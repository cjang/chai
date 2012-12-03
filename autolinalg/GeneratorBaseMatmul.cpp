// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "CodeStatements.hpp"
#include "GeneratorBaseMatmul.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

using namespace AutoParamsMatmul;

////////////////////////////////////////
// matrix matrix multiply

GeneratorBaseMatmul::~GeneratorBaseMatmul(void) { }

string GeneratorBaseMatmul::assignMAD(
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

ostream& GeneratorBaseMatmul::assignMAD(
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

string GeneratorBaseMatmul::kernelForename(void) const
{
    return "matmul";
}

bool GeneratorBaseMatmul::validParams(void) const
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

bool GeneratorBaseMatmul::getParams(vector<size_t>& params) const
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

string GeneratorBaseMatmul::exogenousKey(void) const
{
    stringstream ss;

    ss << kernelForename()
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

void GeneratorBaseMatmul::toggleParamMarks(void)
{
    KernelBaseMatmul::toggleParamMarks();
}

bool GeneratorBaseMatmul::getParams(set< vector<size_t> >& paramSet)
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

void GeneratorBaseMatmul::setParams(const vector<size_t>& params)
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

string GeneratorBaseMatmul::kernelName(void) const
{
    return KernelBaseMatmul::kernelName();
}

vector<size_t> GeneratorBaseMatmul::globalWorkItems(void) const
{
    vector<size_t> dims;
    dims.push_back(N() / blockW());
    dims.push_back(M() / blockH());
    return dims;
}

vector<size_t> GeneratorBaseMatmul::localWorkItems(void) const
{
    vector<size_t> dims;
    dims.push_back(groupW());
    dims.push_back(groupH());
    return dims;
}

size_t GeneratorBaseMatmul::numberFlops(void) const
{
    if (general())
        return batching() * 2 * M() * N() * (K() + 1);
    else
        return batching() * M() * N() * (2 * K() - 1);
}

}; // namespace chai_internal
