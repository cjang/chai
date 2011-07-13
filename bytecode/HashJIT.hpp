// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_HASH_JIT_HPP_
#define _CHAI_HASH_JIT_HPP_

#include <cstddef>
#include <set>
#include <stdint.h>
#include <vector>

#include "BC.hpp"
#include "Stak.hpp"
#include "Visit.hpp"

namespace chai_internal {

////////////////////////////////////////
// more complex bytecode hashing used
// by the JIT middle-end

class HashJIT : public Visit<BC>
{
    // hash code source text
    std::vector< uint32_t > _byteText;

    // keep track of all op codes visited
    std::set< uint32_t >    _opCodes;

    // keep track of all variables visited
    std::set< uint32_t >    _variables;

    // keep track of ptr index into boxed array memory
    void*                   _ptrIndex;

public:
    HashJIT(void);

    const std::set< uint32_t >& opCodes(void) const;
    const std::set< uint32_t >& variables(void) const;

    uint64_t code(void) const;
    size_t ptrIndex(void) const;
    void push(const uint32_t);

    void visit(const uint32_t variable, const uint32_t version);
    void visit(const uint32_t opCode);
    void visit(const double scalar);
    void visit(void* ptr);
};

}; // namespace chai_internal

#endif
