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
    // codes for basic data types
    static const size_t UInt32 = 0;
    static const size_t Int32  = 1;
    static const size_t Float  = sizeof(float);
    static const size_t Double = sizeof(double);

    // code                baseType   sizeof  vecLength
    // 0                   uint32_t   4       4
    // 1                   int32_t    4       4
    // 4 = sizeof(float)   float      4       4
    // 8 = sizeof(double)  double     8       2

    // check if precision is valid size code
    static bool validCode(const size_t PREC);

    // number of ways in 128 bits (i.e. double2, float4, int4, uint4)
    static size_t vecLength(const size_t PREC);

    // number of bytes for basic type
    static size_t sizeOf(const size_t PREC);

    // is this a floating point type?
    static bool isTypeFP(const size_t PREC);

    // change data type to precision size code
    template <typename SCALAR> static size_t getCode(void);

    // change precision type code to printable string name
    static std::string getName(const size_t PREC);

    // width padding
    static size_t padWidth(const size_t PREC, const size_t W);
};

}; // namespace chai_internal

#endif
