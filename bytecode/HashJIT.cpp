// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "HashJIT.hpp"
#include "TEA.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// more complex bytecode hashing used
// by the JIT middle-end

HashJIT::HashJIT(void)
    : _ptrIndex(NULL)
{
    // extra padding if odd number of 32 bit words
    _byteText.push_back(0);
}

const set< uint32_t >& HashJIT::opCodes(void) const
{
    return _opCodes;
}

const set< uint32_t >& HashJIT::variables(void) const
{
    return _variables;
}

uint64_t HashJIT::code(void) const
{
    const size_t startIndex = (0 == _byteText.size() % 2) ? 0 : 1;
    return TEA::hash(
               reinterpret_cast< const uint64_t* >(&_byteText[startIndex]),
               _byteText.size() / 2);
}

size_t HashJIT::ptrIndex(void) const
{
    return _ptrIndex ? reinterpret_cast< size_t >(_ptrIndex) : -1;
}

void HashJIT::push(const uint32_t extra)
{
    _byteText.push_back(extra);
}

void HashJIT::visit(const uint32_t variable, const uint32_t version)
{
    _byteText.push_back(variable);
    _variables.insert(variable);
}

void HashJIT::visit(const uint32_t opCode)
{
    _byteText.push_back(opCode);
    _opCodes.insert(opCode);
}

void HashJIT::visit(const double scalar)
{
    const uint64_t tmp64 = static_cast<uint64_t>(scalar);
    const uint32_t* ptr32 = reinterpret_cast< const uint32_t* >(&tmp64);
    _byteText.push_back(ptr32[0]);
    _byteText.push_back(ptr32[1]);
}

void HashJIT::visit(void* ptr)
{
    _byteText.push_back( reinterpret_cast<size_t>(ptr) );
    _ptrIndex = ptr;
}

}; // namespace chai_internal
