// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <stdint.h>
#include <stdlib.h>

#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

template <> uint32_t posrand<uint32_t>(void)
{
    long int a;
    while (0 == (a = lrand48())) ; // try again until non-zero
    return static_cast<uint32_t>(a >> 24); // 8 bit value to avoid overflow
}

template <> int32_t posrand<int32_t>(void)
{
    long int a;
    while (0 == (a = lrand48())) ; // try again until non-zero
    return static_cast<int32_t>(a >> 24); // 8 bit value to avoid overflow
}

template <> float posrand<float>(void)
{
    double a;
    while (0 == (a = drand48())) ; // try again until non-zero
    return static_cast<float>(a);
}

template <> double posrand<double>(void)
{
    double a;
    while (0 == (a = drand48())) ; // try again until non-zero
    return a;
}

}; // namespace chai_internal
