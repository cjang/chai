// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_ARRAY_BUF_UTIL_HPP_
#define _CHAI_ARRAY_BUF_UTIL_HPP_

namespace chai_internal {

template <typename SCALAR_TYPE> SCALAR_TYPE posrand(void);

template <typename SCALAR_TYPE>
void fillrand(SCALAR_TYPE *outM,
              const size_t length)
{
    for (size_t i = 0; i < length; i++)
        outM[i] = posrand<SCALAR_TYPE>();
}

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
