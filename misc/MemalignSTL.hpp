// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_MEMALIGN_STL_HPP_
#define _CHAI_MEMALIGN_STL_HPP_

#include <memory>
#include <vector>
#include <stdlib.h>

namespace chai_internal {

static const size_t MEMORY_ALIGNMENT = 8;

template <typename T>
struct aligned_allocator : std::allocator<T>
{
    T* allocate(size_t n)
    {
        T *p;
        return posix_memalign((void**)&p, sizeof(T), n * sizeof(T))
            ? NULL // failure
            : p;   // success
    }

    void deallocate(T *p, size_t)
    {
        free(p);
    }
};

template <typename T>
struct Type
{
    typedef std::vector<T, aligned_allocator<T> > aligned_vector;
};

}; // namespace chai_internal

#endif
