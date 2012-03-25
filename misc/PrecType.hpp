// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_PREC_TYPE_HPP_
#define _CHAI_PREC_TYPE_HPP_

#include <cstddef>
#include <string>

namespace chai_internal {

////////////////////////////////////////
// Basic array data types

class PrecType
{
public:
    // size codes for basic data types
    static const size_t UInt32 = 0;
    static const size_t Int32  = 1;
    static const size_t Float  = sizeof(float);
    static const size_t Double = sizeof(double);

    // sizeCode            baseType   sizeof  vecLength
    // 0                   uint32_t   4       4
    // 1                   int32_t    4       4
    // 4 = sizeof(float)   float      4       4
    // 8 = sizeof(double)  double     8       2

    // check if precision is valid size code
    static bool validSizeCode(const size_t precision);

    // number of ways in 128 bits (i.e. double2, float4, int4, uint4)
    static size_t vecLength(const size_t precision);

    // number of bytes for basic type
    static size_t sizeOf(const size_t precision);

    // is this a floating point type?
    static bool isTypeFP(const size_t precision);

    // change data type to precision size code
    template <typename SCALAR> static size_t getSizeCode(void);

    // change precision type code to printable string name
    static std::string getPrimitiveName(const size_t precision);
};

}; // namespace chai_internal

#endif
