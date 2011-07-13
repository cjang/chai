// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "HashBC.hpp"
#include "TEA.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// simple bytecode hashing
// for trace scheduling

HashBC::HashBC(void)
{
    // extra padding if odd number of 32 bit words
    _byteText.push_back(0);
}

uint64_t HashBC::code(void) const
{
    const size_t startIndex = (0 == _byteText.size() % 2) ? 0 : 1;
    return TEA::hash(
               reinterpret_cast< const uint64_t* >(&_byteText[startIndex]),
               _byteText.size() / 2);
}

void HashBC::push(const uint32_t extra)
{
    _byteText.push_back(extra);
}

void HashBC::visit(const uint32_t variable, const uint32_t version)
{
    _byteText.push_back(variable);
    _byteText.push_back(version);
}

void HashBC::visit(const uint32_t opCode)
{
    _byteText.push_back(opCode);
}

void HashBC::visit(const double scalar)
{
    const uint64_t tmp64 = static_cast<uint64_t>(scalar);
    const uint32_t* ptr32 = reinterpret_cast< const uint32_t* >(&tmp64);
    _byteText.push_back(ptr32[0]);
    _byteText.push_back(ptr32[1]);
}

void HashBC::visit(void* ptr)
{
    _byteText.push_back( reinterpret_cast<size_t>(ptr) );
}

}; // namespace chai_internal
