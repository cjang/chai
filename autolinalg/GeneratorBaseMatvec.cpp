// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "CodeStatements.hpp"
#include "GeneratorBaseMatvec.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

using namespace AutoParamsMatmul;

////////////////////////////////////////
// matrix vector multiply

GeneratorBaseMatvec::~GeneratorBaseMatvec(void) { }

string GeneratorBaseMatvec::assignMAD(
           const vector< PrivateVar >& accum,
           const vector< PrivateVar >& valueA,
           const vector< PrivateVar >& valueB,
           const size_t j,        // output row
           const size_t l ) const // inner product column
{
    const size_t vlenA = valueA[0].vecLength();
    const size_t vlenB = valueB[0].vecLength();
    const size_t vlenC = accum[0].vecLength();

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

string GeneratorBaseMatvec::kernelForename(void) const
{
    return "matvec";
}

bool GeneratorBaseMatvec::validParams(void) const
{
    bool blockingOk;

    if (transposeA())
    {
        blockingOk =

            // block width must fit along rows of transposed matrix
            // and the output vector
            0 == M() % (groupSize() * blockW()) &&
            groupSize() * blockW() <= M() &&

            // work item group size must fit input vector
            0 == N() % (groupSize() * effVecLengthB()) &&
            groupSize() * effVecLengthB() <= N();
    }
    else
    {
        blockingOk =

            // block width must fit along rows of matrix and the input vector
            0 == N() % (groupSize() * blockW()) &&
            groupSize() * blockW() <= N() &&

            // work item group size must fit output vector
            0 == M() % (groupSize() * effVecLengthC()) &&
            groupSize() * effVecLengthC() <= M();
    }

    return
        // array types of A, B, C
        validArrayType(precA(), vecLengthA()) &&
        validArrayType(precB(), vecLengthB()) &&
        validArrayType(precC(), vecLengthC()) &&

        // if GEMV, then C must be read-write (can not be an image)
        !(general() && 0 == vecLengthC()) &&

        // batched kernels
        batching() > 0 &&

        // inner and outer blocking compatible with problem dimensions
        blockingOk &&

        // extra parameter
        extraParam() < totalVariations();
}

bool GeneratorBaseMatvec::getParams(vector<size_t>& params) const
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

        // batched kernels
        params.push_back(batching());
        if (getDifferentData()) params.push_back(0);
        else if (getSameDataMatrixA()) params.push_back(1);
        else if (getSameDataMatrixB()) params.push_back(2);

        // GEMM or pure matrix multiply only
        params.push_back(general());

        // matrix dimensions
        params.push_back(M());
        params.push_back(N());

        // data layout
        params.push_back(transposeA());

        // work group
        params.push_back(groupSize());

        // inner blocking
        params.push_back(blockW());

        // extra parameter
        params.push_back(extraParam());
    }

    return rc;
}

string GeneratorBaseMatvec::exogenousKey(void) const
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
       << "_" << M()
       << "_" << N();

    return ss.str();
}

void GeneratorBaseMatvec::toggleParamMarks(void)
{
    KernelBaseMatmul::toggleParamMarks();
}

bool GeneratorBaseMatvec::getParams(set< vector<size_t> >& paramSet)
{
    // exogenous: batching (default is 1: no batching)
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
        (batching() > 0) &&
        PrecType::validCode(precA()) &&
        PrecType::validCode(precB()) &&
        PrecType::validCode(precC()) &&
        (0 != M()) &&
        (0 != N());

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

void GeneratorBaseMatvec::setParams(const vector<size_t>& params)
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
    setDimensions(M, N);

    // data layout
    const size_t transposeA = params[index++];
    setDataLayout(transposeA);

    // work group
    const size_t groupSize = params[index++];
    setWorkGroup(groupSize);

    // inner blocking
    const size_t blockW = params[index++];
    setInnerBlocking(1, blockW);

    // extra parameter
    const size_t extraParam = params[index++];
    setExtraParam(extraParam);
}

string GeneratorBaseMatvec::kernelName(void) const
{
    return KernelBaseMatmul::kernelName();
}

vector<size_t> GeneratorBaseMatvec::globalWorkItems(void) const
{
    vector<size_t> dims;

    if (transposeA())
    {
        dims.push_back(M() / blockW());
    }
    else
    {
        dims.push_back(M() / effVecLengthC());
    }

    return dims;
}

vector<size_t> GeneratorBaseMatvec::localWorkItems(void) const
{
    vector<size_t> dims;

    dims.push_back(groupSize());

    return dims;
}

size_t GeneratorBaseMatvec::numberFlops(void) const
{
    if (general())
        return batching() * M() * (2 * N() + 1);
    else
        return batching() * M() * (2 * N() - 1);
}

}; // namespace chai_internal
