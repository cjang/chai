// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_R123_UTIL_HPP_
#define _CHAI_R123_UTIL_HPP_

#include <string>

namespace chai_internal {

////////////////////////////////////////
// Random123 OpenCL utility functions

class R123Util
{
protected:
    const std::string _M_RAN_INVM32;
    const std::string _M_RAN_INVM64;
    const std::string _SR123;

    // uint ## PR ## _t
    const char* uit(const size_t PR) const;

    // VL ## x ## PR
    const char* vxp(const size_t VL, const size_t PR) const;

    // PR ## x ## VL
    const char* pxv(const size_t VL, const size_t PR) const;

    R123Util(void);

public:
    virtual ~R123Util(void);
};

}; // namespace chai_internal

#endif
