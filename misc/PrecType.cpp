// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdint.h>

#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// Basic array data types

bool PrecType::validCode(const size_t PREC)
{
    switch (PREC)
    {
        case (PrecType::UInt32) :
        case (PrecType::Int32) :
        case (PrecType::Float) :
        case (PrecType::Double) :
            return true;

        default :
            return false;
    }
}

size_t PrecType::vecLength(const size_t PREC)
{
    // assuming 128 bits
    switch (PREC)
    {
        // 4-way
        case (PrecType::UInt32) :
        case (PrecType::Int32) :
        case (PrecType::Float) :
            return 4;

        // 2-way
        case (PrecType::Double) :
            return 2;

        // should never happen
        default:
            return 0;
    }
}

size_t PrecType::sizeOf(const size_t PREC)
{
    switch (PREC)
    {
        case (PrecType::UInt32) :
        case (PrecType::Int32) :
        case (PrecType::Float) :
            return 4;

        case (PrecType::Double) :
            return 8;

        // should never happen
        default:
            return 0;
    }
}

bool PrecType::isTypeFP(const size_t PREC)
{
    switch (PREC)
    {
        case (PrecType::UInt32) :
        case (PrecType::Int32) :
            return false;

        case (PrecType::Float) :
        case (PrecType::Double) :
            return true;

        // should never happen
        default:
            return false;
    }
}

template <> size_t PrecType::getCode<uint32_t>(void)
{
    return PrecType::UInt32;
}

template <> size_t PrecType::getCode<int32_t>(void)
{
    return PrecType::Int32;
}

template <> size_t PrecType::getCode<float>(void)
{
    return PrecType::Float;
}

template <> size_t PrecType::getCode<double>(void)
{
    return PrecType::Double;
}

string PrecType::getName(const size_t PREC)
{
    switch (PREC)
    {
        case (PrecType::UInt32) : return "uint";
        case (PrecType::Int32) : return "int";
        case (PrecType::Float) : return "float";
        case (PrecType::Double) : return "double";

        // should never happen
        default : return "";
    }
}

size_t PrecType::padWidth(const size_t PREC, const size_t W)
{
    const size_t stdVecLen = PrecType::vecLength(PREC);

    // if not even multiple of standard vector length
    const bool padNeeded = 0 != (W % stdVecLen);

    // width is forced to be a multiple of standard vector length
    const size_t width = padNeeded
                             ? stdVecLen * (1 + (W / stdVecLen))
                             : W;

    return width;
}

}; // namespace chai_internal
