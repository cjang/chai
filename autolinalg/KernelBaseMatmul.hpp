// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_KERNEL_BASE_MATMUL_HPP_
#define _CHAI_KERNEL_BASE_MATMUL_HPP_

#include <set>
#include <string>
#include <vector>

#include "AutoParamsMatmul.hpp"
#include "KernelBase.hpp"

namespace chai_internal {

////////////////////////////////////////
// Base for matmul kernel generators

namespace AutoParamsMatmul {
    class Batching;
    class DataLayout;
    class Dimensions;
    class ExtraParam;
    class General;
    class InnerBlocking;
    class Precision;
    class VectorLength;
    class WorkGroup;
};

class KernelBaseMatmul : public KernelBase,
                         protected AutoParamsMatmul::Batching,
                         protected AutoParamsMatmul::General,
                         protected AutoParamsMatmul::Precision,
                         protected AutoParamsMatmul::Dimensions,
                         protected AutoParamsMatmul::DataLayout,
                         protected AutoParamsMatmul::WorkGroup,
                         protected AutoParamsMatmul::InnerBlocking,
                         protected AutoParamsMatmul::VectorLength,
                         protected AutoParamsMatmul::ExtraParam
{
public:
    // set parameters
    using AutoParamsMatmul::Batching::setBatching;
    using AutoParamsMatmul::Batching::setDifferentData;
    using AutoParamsMatmul::Batching::setSameDataMatrixA;
    using AutoParamsMatmul::Batching::setSameDataMatrixB;
    using AutoParamsMatmul::General::setGeneral;
    using AutoParamsMatmul::Precision::setPrecision;
    using AutoParamsMatmul::Dimensions::setDimensions;
    using AutoParamsMatmul::DataLayout::setDataLayout;
    using AutoParamsMatmul::WorkGroup::setWorkGroup;
    using AutoParamsMatmul::InnerBlocking::setInnerBlocking;
    using AutoParamsMatmul::VectorLength::setVectorLength;
    using AutoParamsMatmul::ExtraParam::setExtraParam;

    // EM
    using AutoParamsMatmul::WorkGroup::doNotOptimizeWorkGroup;
    using AutoParamsMatmul::WorkGroup::activeOptimizeWorkGroup;
    using AutoParamsMatmul::WorkGroup::inactiveOptimizeWorkGroup;
    using AutoParamsMatmul::InnerBlocking::doNotOptimizeInnerBlocking;
    using AutoParamsMatmul::InnerBlocking::activeOptimizeInnerBlocking;
    using AutoParamsMatmul::InnerBlocking::inactiveOptimizeInnerBlocking;
    using AutoParamsMatmul::VectorLength::doNotOptimizeVectorLength;
    using AutoParamsMatmul::VectorLength::activeOptimizeVectorLength;
    using AutoParamsMatmul::VectorLength::inactiveOptimizeVectorLength;
    using AutoParamsMatmul::ExtraParam::doNotOptimizeExtraParam;
    using AutoParamsMatmul::ExtraParam::activeOptimizeExtraParam;
    using AutoParamsMatmul::ExtraParam::inactiveOptimizeExtraParam;

    // get parameters
    using AutoParamsMatmul::VectorLength::vecLengthA;
    using AutoParamsMatmul::VectorLength::vecLengthB;
    using AutoParamsMatmul::VectorLength::vecLengthC;
    using AutoParamsMatmul::Precision::precA;
    using AutoParamsMatmul::Precision::precB;
    using AutoParamsMatmul::Precision::precC;

protected:
    virtual ~KernelBaseMatmul(void);

    bool validArrayType(const size_t PREC,
                        const size_t vecLen) const;

    size_t effVecLengthA(void) const;
    size_t effVecLengthB(void) const;
    size_t effVecLengthC(void) const;

    bool useMADFunction(void) const;

public:
    virtual std::string kernelForename(void) const = 0;

    virtual bool validParams(void) const = 0;
    virtual bool getParams(std::vector<size_t>& params) const = 0;

    virtual void toggleParamMarks(void) = 0;

    virtual std::string exogenousKey(void) const = 0;
    virtual std::string kernelName(void) const = 0;
};

}; // namespace chai_internal

#endif
