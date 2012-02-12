// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_CODE_TYPE_HPP_
#define _CHAI_CODE_TYPE_HPP_

#include <ostream>
#include <string>

namespace chai_internal {

////////////////////////////////////////
// type related stuff

class NameOf
{
    // precision: 0 is uint
    //            1 is int
    //            sizeof(float) is float
    //            sizeof(double) is double
    // vectorLength 1, 2, 4 is memory buffer
    // writable: true is nonconst, false is const
    static void memvar(std::ostream&,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool writable);

public:
    // declare kernel global array variable
    static std::string globalvar(const size_t precision,
                                 const size_t vectorLength,
                                 const bool writable);

    // declare kernel local array variable
    static std::string localvar(const size_t precision,
                             const size_t vectorLength,
                             const bool writable = false);

    // declare kernel private variable
    static std::string privatevar(const size_t precision,
                                  const size_t vectorLength,
                                  const bool writable);
};

// OpenCL pragmas required for specific scalar types
std::string pragma_extension(const size_t precision,
                             const size_t deviceIndex);

}; // namespace chai_internal

#endif
