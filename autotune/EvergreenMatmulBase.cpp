// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <map>
#include <sstream>

#include "CodeControlFlow.hpp"
#include "CodeFunction.hpp"
#include "CodeImageSample.hpp"
#include "CodeStatements.hpp"
#include "CodeType.hpp"
#include "CodeValues.hpp"
#include "EvergreenMatmulBase.hpp"
#include "PrecType.hpp"
#include "UtilFuns.hpp"

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

bool MatmulBase::validArrayType(const size_t PREC,
                                const size_t vecLen) const
{
    if ( ! PrecType::validCode(PREC) ||
         ! (0 == vecLen || 1 == vecLen || 2 == vecLen || 4 == vecLen) )
    {
        return false;
    }

    if (0 == vecLen) // image
    {
        if (0 != blockW() % PrecType::vecLength(PREC))
        {
            return false;
        }
    }
    else // memory buffer
    {
        if (0 != blockW() % vecLen)
        {
            return false;
        }
    }

    return true;
}

size_t MatmulBase::effVecLengthA(void) const
{
    if (0 == vecLengthA())
    {
        return PrecType::vecLength(precA());
    }
    else
    {
        return vecLengthA();
    }
}

size_t MatmulBase::effVecLengthB(void) const
{
    if (0 == vecLengthB())
    {
        return PrecType::vecLength(precB());
    }
    else
    {
        return vecLengthB();
    }
}

size_t MatmulBase::effVecLengthC(void) const
{
    if (0 == vecLengthC())
    {
        return PrecType::vecLength(precC());
    }
    else
    {
        return vecLengthC();
    }
}

bool MatmulBase::useMADFunction(void) const
{
    return PrecType::isTypeFP(precA()) &&
           PrecType::isTypeFP(precB()) &&
           PrecType::isTypeFP(precC());
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
             it = params.begin(); it != params.end(); it++)
        {
            ss << "_" << *it;
        }
    }

    return ss.str();
}

}; // namespace Evergreen

}; // namespace chai_internal
