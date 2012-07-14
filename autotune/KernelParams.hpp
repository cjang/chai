// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_KERNEL_PARAMETERS_HPP_
#define _CHAI_KERNEL_PARAMETERS_HPP_

#include <vector>

#include "CodeInterfaces.hpp"
#include "CodeValues.hpp"

namespace chai_internal {

////////////////////////////////////////
// Batching (exogenous)

class Batching
{
    // number of computational kernels to pack into one GPU kernel
    size_t _batchNum;

    // if one matrix input argument remains constant across kernels/threads
    enum DataAcrossTraces
    {
        DIFFERENT_DATA,
        SAME_DATA_MATRIX_A,
        SAME_DATA_MATRIX_B
    };

    DataAcrossTraces _dataAcrossTraces;

    // did the number of kernels change?
    bool _changed;

public:
    Batching(void);

    void setBatching(const size_t batchNum);
    void setDifferentData(void);
    void setSameDataMatrixA(void);
    void setSameDataMatrixB(void);

    bool batchingChanged(void) const;

    size_t batching(void) const;
    bool getDifferentData(void) const;
    bool getSameDataMatrixA(void) const;
    bool getSameDataMatrixB(void) const;
};

////////////////////////////////////////
// General (exogenous)

class General
{
    // GEMM or GEMV if true, otherwise plain MM or MV if false (default)
    bool _general;

    // buffers are different for matrix multiply and GEMM
    bool _changed;

public:
    General(void);

    void setGeneral(const bool value);

    bool generalChanged(void) const;

    bool general(void) const;
};

////////////////////////////////////////
// Precision (exogenous)

class Precision
{
    // 0 is uint32_t
    // 1 is int32_t
    // 4 = sizeof(float) is single precision
    // 8 = sizeof(double) is double precision
    size_t _precA;
    size_t _precB;
    size_t _precC;

    // did any matrix change between single and double precision?
    bool _changed;

public:
    Precision(void);

    void setPrecision(const size_t A, const size_t B, const size_t C);

    bool precisionChanged(void) const;

    size_t precA(void) const;
    size_t precB(void) const;
    size_t precC(void) const;
};

////////////////////////////////////////
// Dimensions (exogenous)

class Dimensions
{
    // C = AB where A is MxK, B is KxN, C is MxN
    // y = Ax where A is MxN, x is Nx1, y is Mx1
    size_t _M;
    size_t _N;
    size_t _K;

    // indicates memory must be resized
    bool _changed;

public:
    Dimensions(void);

    void setDimensions(const size_t M, const size_t N);
    void setDimensions(const size_t M, const size_t N, const size_t K);

    bool dimensionsChanged(void) const;

    size_t M(void) const;
    size_t N(void) const;
    size_t K(void) const;
};

////////////////////////////////////////
// DataLayout (exogenous)

class DataLayout
{
    // transpose  matrix data layout
    // ---------  ------------------
    // true       column major
    // false      row major
    bool _transposeA;
    bool _transposeB;

    // indicates images must be reallocated
    bool _changed;

public:
    DataLayout(void);

    void setDataLayout(const bool A);
    void setDataLayout(const bool A, const bool B);

    bool layoutChanged(void) const;

    bool transposeA(void) const;
    bool transposeB(void) const;
};

////////////////////////////////////////
// EndogenousBase

class EndogenousBase
{
    enum OptimizeState
    {
        DO_NOT_OPTIMIZE,
        ACTIVE,
        INACTIVE
    };

    OptimizeState _optimizeThisParam;

public:
    EndogenousBase(void);

    void doNotOptimize(void);
    void activeOptimize(void);
    void inactiveOptimize(void);
    void toggleActive(void);

    bool isActive(void) const;
};

////////////////////////////////////////
// WorkGroup (endogenous)

class WorkGroup : public EndogenousBase
{
    // work group dimensions, corresponds to warp/wavefront
    size_t _H;
    size_t _W;

    // add one to avoid local memory bank conflicts
    static const size_t LOCALMEM_PAD = 1;

public:
    // work item IDs
    const ConstantValue globalCol;
    const ConstantValue globalRow;
    const ConstantValue groupRow;
    const ConstantValue groupCol;
    const ConstantValue localRow;
    const ConstantValue localCol;

    WorkGroup(void);

    void setWorkGroup(const size_t H, const size_t W);
    void setWorkGroup(const size_t SZ);

    void doNotOptimizeWorkGroup(void);
    void activeOptimizeWorkGroup(void);
    void inactiveOptimizeWorkGroup(void);

    size_t groupH(void) const;
    size_t groupW(void) const;
    size_t groupSize(void) const;

    // for memory buffer kernels
    size_t localH(void) const;
    size_t localW(void) const;
    size_t localSize(void) const;
};

////////////////////////////////////////
// InnerBlocking (endogenous)

class InnerBlocking : public EndogenousBase
{
    size_t _H;
    size_t _W;

public:
    InnerBlocking(void);

    // blocking is height x width
    void setInnerBlocking(const size_t H, const size_t W);

    void doNotOptimizeInnerBlocking(void);
    void activeOptimizeInnerBlocking(void);
    void inactiveOptimizeInnerBlocking(void);

    size_t blockH(void) const;
    size_t blockW(void) const;
};

////////////////////////////////////////
// VectorLength (endogenous)

class VectorLength : public EndogenousBase
{
    // -1 is unset
    // 0 is image (float4 or double2)
    // 1 is memory buffer with vector length 1
    // 2 is memory buffer with vector length 2
    // 4 is memory buffer with vector length 4
    size_t _vecLenA;
    size_t _vecLenB;
    size_t _vecLenC;

    // did one of the arrays change?
    bool _changed;

public:
    VectorLength(void);

    void setVectorLength(const size_t A, const size_t B, const size_t C);

    void doNotOptimizeVectorLength(void);
    void activeOptimizeVectorLength(void);
    void inactiveOptimizeVectorLength(void);

    bool vectorLengthChanged(void) const;

    size_t vecLengthA(void) const;
    size_t vecLengthB(void) const;
    size_t vecLengthC(void) const;
};

////////////////////////////////////////
// AttrAutoVec (endogenous)
// NOTE: this is not currently used

class AttrAutoVec
{
    // kernel vector attribute hint is not supported on all platforms
    bool _useAttrAutoVec; // default value is true

public:
    AttrAutoVec(void);

    void setUseAttrAutoVec(const bool value);
    bool getUseAttrAutoVec(void) const;
};

////////////////////////////////////////
// ExtraParam (pure endogenous)

class ExtraParamObserver;

class ExtraParam : public EndogenousBase
{
    size_t _extraParam;
    size_t _totalVariations; // 0 <= _extraParam < _totalVariations

    std::vector<size_t> _observerParams;

    std::vector<ExtraParamObserver*> _observers;

    std::vector<size_t> _numberVariations;

public:
    ExtraParam(void);

    ExtraParam& getExtraParam(void);

    void addObserver(ExtraParamObserver* observer);

    void setExtraParam(const size_t value);

    void doNotOptimizeExtraParam(void);
    void activeOptimizeExtraParam(void);
    void inactiveOptimizeExtraParam(void);

    std::vector<size_t> extraParamDetail(void) const;
    size_t extraParam(void) const;
    size_t totalVariations(void) const;
};

class ExtraParamObserver
{
    const size_t _numberVariations;

    size_t       _paramValue;

protected:
    size_t getParam(void) const;

public:
    ExtraParamObserver(const size_t numberVariations,
                       ExtraParam& subject);

    size_t numberVariations(void) const;
    void setParam(const size_t value);
};

////////////////////////////////////////
// ParamInlineDim (pure endogenous)

struct ParamInlineDim : public ExtraParamObserver
{
    ParamInlineDim(ExtraParam& subject);

    // dimensions are inlined constants or passed as kernel arguments
    bool inlineDim(void) const;
};

////////////////////////////////////////
// ParamLoopOrder (pure endogenous)

struct ParamLoopOrder : public ExtraParamObserver
{
    ParamLoopOrder(ExtraParam& subject);

    // inner product accumulation loop order, 3! permutations of (j,k,l)
    size_t loopOrder(void) const;
};

////////////////////////////////////////
// ParamGlobalID (pure endogenous)

struct ParamGlobalID : public ExtraParamObserver
{
    ParamGlobalID(ExtraParam& subject);

    // use global or group/local ID
    bool globalID(void) const;
};

}; // namespace chai_internal

#endif
