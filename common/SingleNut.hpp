// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SINGLE_NUT_HPP_
#define _CHAI_SINGLE_NUT_HPP_

#include <map>
#include <stdint.h>
#include <vector>

#include "chai/RefCnt.hpp"
#include "FrontMem.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// versioned array values for a single
// trace

class SingleNut : public RefObj
{
    // all values ever stored in this nut
    RefSet _refs;

    // version -> intermediate value
    std::map< uint32_t, std::vector< FrontMem* > > _versionMem;

    // version -> AST variable
    std::map< uint32_t, AstVariable* > _versionVar;

    const std::vector< FrontMem* > _dummy;

public:
    SingleNut(void);

    const std::vector< FrontMem* >& getNutMem(const uint32_t version);
    void setNutMem(const uint32_t version, const std::vector< FrontMem* >&);

    AstVariable* getNutVar(const uint32_t version);
    void setNutVar(const uint32_t version, AstVariable*);
};

}; // namespace chai_internal

#endif
