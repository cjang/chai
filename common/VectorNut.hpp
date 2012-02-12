// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_VECTOR_NUT_HPP_
#define _CHAI_VECTOR_NUT_HPP_

#include <stdint.h>
#include <vector>

#include "FrontMem.hpp"
#include "SingleNut.hpp"

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// read and write to array variable
// nuts as vectors

class VectorNut
{
    // array variable nuts store data values (and AST objects in alternative)
    std::vector< SingleNut* > _nutVector;

public:
    VectorNut(void);

    void push(SingleNut*);

    std::vector< FrontMem* > getNutMem(const uint32_t version);
    AstVariable* getNutVar(const uint32_t version);

    void setNutMem(const uint32_t version, const std::vector< FrontMem* >&);
    void setNutVar(const uint32_t version, AstVariable*);
};

}; // namespace chai_internal

#endif
