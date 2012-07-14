// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_UTIL_FUNS_HPP_
#define _CHAI_UTIL_FUNS_HPP_

#include <cstddef>

namespace chai_internal {

////////////////////////////////////////
// simple utility functions

// minimum of two values
template <typename X, typename Y, typename Z>
X min(const Y a, const Z b)
{
    return (a > b) ? b : a;
}

// maximum of two values
template <typename X, typename Y, typename Z>
X max(const Y a, const Z b)
{
    return (a > b) ? a : b;
}

// maximum of three values
template <typename X, typename Y, typename Z, typename A>
X max(const Y a, const Z b, const A c)
{
    return ((a > b) ? a : b) > c
               ? ((a > b) ? a : b)
               : c;
}

// positive random numbers
template <typename SCALAR_TYPE> SCALAR_TYPE posrand(void);

// fill buffer with positive random numbers
template <typename SCALAR_TYPE>
void fillrand(SCALAR_TYPE *outM,
              const size_t length)
{
    for (size_t i = 0; i < length; i++)
        outM[i] = posrand<SCALAR_TYPE>();
}

// fill buffer with constant value
template <typename SCALAR_TYPE>
void fillconst(SCALAR_TYPE *outM,
               const SCALAR_TYPE value,
               const size_t length)
{
    for (size_t i = 0; i < length; i++)
        outM[i] = value;
}

}; // namespace chai_internal

#endif
