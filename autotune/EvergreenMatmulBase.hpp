// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_EVERGREEN_MATMUL_BASE_HPP_
#define _CHAI_EVERGREEN_MATMUL_BASE_HPP_

#include <set>
#include <string>
#include <vector>

#include "KernelBase.hpp"
#include "KernelParams.hpp"

namespace chai_internal {

namespace Evergreen {

////////////////////////////////////////
// Evergreen::MatmulBase

class MatmulBase : public KernelBase,
                   protected Packing,
                   protected General,
                   protected Precision,
                   protected Dimensions,
                   protected DataLayout,
                   protected WorkGroup,
                   protected InnerBlocking,
                   protected VectorLength,
                   protected ExtraParam
{
public:
    static std::string deviceFamily(void);

    // set parameters
    using Packing::setPacking;
    using Packing::setDifferentData;
    using Packing::setSameDataMatrixA;
    using Packing::setSameDataMatrixB;
    using General::setGeneral;
    using Precision::setPrecision;
    using Dimensions::setDimensions;
    using DataLayout::setDataLayout;
    using WorkGroup::setWorkGroup;
    using InnerBlocking::setInnerBlocking;
    using VectorLength::setVectorLength;
    using ExtraParam::setExtraParam;

    // EM
    using WorkGroup::doNotOptimizeWorkGroup;
    using WorkGroup::activeOptimizeWorkGroup;
    using WorkGroup::inactiveOptimizeWorkGroup;
    using InnerBlocking::doNotOptimizeInnerBlocking;
    using InnerBlocking::activeOptimizeInnerBlocking;
    using InnerBlocking::inactiveOptimizeInnerBlocking;
    using VectorLength::doNotOptimizeVectorLength;
    using VectorLength::activeOptimizeVectorLength;
    using VectorLength::inactiveOptimizeVectorLength;
    using ExtraParam::doNotOptimizeExtraParam;
    using ExtraParam::activeOptimizeExtraParam;
    using ExtraParam::inactiveOptimizeExtraParam;

    // get parameters
    using VectorLength::vectorLengthA;
    using VectorLength::vectorLengthB;
    using VectorLength::vectorLengthC;
    using Precision::precisionA;
    using Precision::precisionB;
    using Precision::precisionC;

protected:
    MatmulBase(void);
    virtual ~MatmulBase(void);

    bool validArrayType(const size_t precision,
                        const size_t vectorLength) const;

    size_t maxPrecision(const size_t precisionA,
                        const size_t precisionB) const;

    size_t maxPrecision(const size_t precisionA,
                        const size_t precisionB,
                        const size_t precisionC) const;

    size_t effVectorLengthA(void) const;
    size_t effVectorLengthB(void) const;
    size_t effVectorLengthC(void) const;

    bool useMADFunction(void) const;

public:
    virtual std::string kernelForename(void) const = 0;

    virtual bool validParams(void) const = 0;
    virtual bool getParams(std::vector<size_t>& params) const = 0;

    virtual void toggleParamMarks(void) = 0;

    virtual std::string exogenousKey(void) const = 0;
    virtual std::string kernelName(void) const = 0;
};

}; // namespace Evergreen

}; // namespace chai_internal

#endif
