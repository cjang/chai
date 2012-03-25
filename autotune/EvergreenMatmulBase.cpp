// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <map>
#include <sstream>

#include "ArrayBufUtil.hpp"
#include "CodeControlFlow.hpp"
#include "CodeFunction.hpp"
#include "CodeImageSample.hpp"
#include "CodeStatements.hpp"
#include "CodeType.hpp"
#include "CodeValues.hpp"
#include "EvergreenMatmulMM.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

namespace Evergreen {

////////////////////////////////////////
// Evergreen::MatmulMMBase

string MatmulBase::deviceFamily(void)
{
    return "evergreen";
}

MatmulBase::MatmulBase(void) { }

MatmulBase::~MatmulBase(void) { }

bool MatmulBase::validArrayType(const size_t precision,
                                const size_t vectorLength) const
{
    if ( ! PrecType::validSizeCode(precision) ||
         ! (0 == vectorLength ||
            1 == vectorLength ||
            2 == vectorLength ||
            4 == vectorLength) )
    {
        return false;
    }

    if (0 == vectorLength) // image
    {
        if (0 != blockWidth() % PrecType::vecLength(precision))
        {
            return false;
        }
    }
    else // memory buffer
    {
        if (0 != blockWidth() % vectorLength)
        {
            return false;
        }
    }

    return true;
}

size_t MatmulBase::maxPrecision(const size_t precisionA,
                                const size_t precisionB) const
{
    // precB:  0  1  4  8
    // precA:
    //     0   U  I  F  D
    //     1   I  I  F  D
    //     4   F  F  F  D
    //     8   D  D  D  D

    return precisionA < precisionB
               ? precisionB
               : precisionA;
}

size_t MatmulBase::maxPrecision(const size_t precisionA,
                                const size_t precisionB,
                                const size_t precisionC) const
{
    return maxPrecision(
               maxPrecision(precisionA, precisionB),
               precisionC );
}

size_t MatmulBase::effVectorLengthA(void) const
{
    if (0 == vectorLengthA())
    {
        return PrecType::vecLength(precisionA());
    }
    else
    {
        return vectorLengthA();
    }
}

size_t MatmulBase::effVectorLengthB(void) const
{
    if (0 == vectorLengthB())
    {
        return PrecType::vecLength(precisionB());
    }
    else
    {
        return vectorLengthB();
    }
}

size_t MatmulBase::effVectorLengthC(void) const
{
    if (0 == vectorLengthC())
    {
        return PrecType::vecLength(precisionC());
    }
    else
    {
        return vectorLengthC();
    }
}

bool MatmulBase::useMADFunction(void) const
{
    return PrecType::isTypeFP(precisionA()) &&
           PrecType::isTypeFP(precisionB()) &&
           PrecType::isTypeFP(precisionC());
}

void MatmulBase::toggleParamMarks(void)
{
    WorkGroup::toggleActive();
    InnerBlocking::toggleActive();
    VectorLength::toggleActive();
    ExtraParam::toggleActive();
}

string MatmulBase::kernelName(void) const
{
    stringstream ss;

    ss << deviceFamily()
       << kernelForename();

    vector< size_t > params;

    if (getParams(params))
    {
        for (vector< size_t >::const_iterator
             it = params.begin();
             it != params.end();
             it++)
        {
            ss << "_" << *it;
        }
    }

    return ss.str();
}

}; // namespace Evergreen

}; // namespace chai_internal
