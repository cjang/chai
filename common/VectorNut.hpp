// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VECTOR_NUT_HPP_
#define _CHAI_VECTOR_NUT_HPP_

#include <stdint.h>
#include <vector>

#include "FrontMem.hpp"
#include "RefCnt.hpp"
#include "SingleNut.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// read and write to array variable
// nuts as vectors

class VectorNut
{
    // all values ever stored in this nut
    RefSet _refs;

    // array variable nuts store data values
    std::vector< SingleNut* > _nutVector;

    // version to AST variable
    std::map< uint32_t, AstVariable* > _versionMap;

public:
    VectorNut(void);

    void push(SingleNut*);

    std::vector< FrontMem* > getNutMem(const uint32_t version);
    AstVariable* getNutVar(const uint32_t version);

    void setNut(const uint32_t version, const std::vector< FrontMem* >&);
    void setNut(const uint32_t version, AstVariable*);
};

}; // namespace chai_internal

#endif
