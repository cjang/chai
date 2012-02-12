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
    if ( ! (sizeof(float) == precision || sizeof(double) == precision) ||
         ! (0 == vectorLength ||
            1 == vectorLength ||
            2 == vectorLength ||
            4 == vectorLength) )
    {
        return false;
    }

    if (0 == vectorLength) // image
    {
        if ( (sizeof(float) == precision && 0 != blockWidth() % 4) ||
             (sizeof(double) == precision && 0 != blockWidth() % 2) )
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

size_t MatmulBase::effVectorLengthA(void) const
{
    return (0 == vectorLengthA() ? (sizeof(float) == precisionA() ? 4 : 2)
                                 : vectorLengthA());
}

size_t MatmulBase::effVectorLengthB(void) const
{
    return (0 == vectorLengthB() ? (sizeof(float) == precisionB() ? 4 : 2)
                                 : vectorLengthB());
}

size_t MatmulBase::effVectorLengthC(void) const
{
    return (0 == vectorLengthC() ? (sizeof(float) == precisionC() ? 4 : 2)
                                 : vectorLengthC());
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
