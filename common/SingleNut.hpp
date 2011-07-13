// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SINGLE_NUT_HPP_
#define _CHAI_SINGLE_NUT_HPP_

#include <map>
#include <stdint.h>

#include "FrontMem.hpp"
#include "RefCnt.hpp"

namespace chai_internal {

////////////////////////////////////////
// versioned array values for a single
// trace

class SingleNut : public RefObj
{
    // all values ever stored in this nut
    RefSet _refs;

    // version -> value
    std::map< uint32_t, FrontMem* > _versionMap;

public:
    SingleNut(void);

    FrontMem* getNut(const uint32_t version);
    void setNut(const uint32_t version, FrontMem*);
};

}; // namespace chai_internal

#endif
