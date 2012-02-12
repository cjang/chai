// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_HASH_BC_HPP_
#define _CHAI_HASH_BC_HPP_

#include <cstddef>
#include <stdint.h>
#include <vector>

#include "chai/BC.hpp"
#include "chai/Stak.hpp"
#include "chai/Visit.hpp"

namespace chai_internal {

////////////////////////////////////////
// simple bytecode hashing
// for trace scheduling

class HashBC : public Visit<BC>
{
    // hash code source text
    std::vector< uint32_t > _byteText;

public:
    HashBC(void);

    uint64_t code(void) const;
    void push(const uint32_t);

    void visit(const uint32_t variable, const uint32_t version);
    void visit(const uint32_t opCode);
    void visit(const double scalar);
    void visit(void* ptr);
};

}; // namespace chai_internal

#endif
