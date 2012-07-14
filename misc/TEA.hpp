// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_TEA_HPP_
#define _CHAI_TEA_HPP_

#include <cstddef>
#include <stdint.h>
#include <string>
#include <vector>

namespace chai_internal {

////////////////////////////////////////
// Tiny Encryption Algorithm
// by David Wheeler and Roger Needham

struct TEA
{
    // David Wagner's new variant implementation
    static void encipher(const uint32_t * const v,
                         uint32_t * const w,
                         const uint32_t * const k);

    // David Wagner's new variant implementation
    static void decipher(const uint32_t * const v,
                         uint32_t * const w,
                         const uint32_t * const k);

    // Davies-Meyer hash function
    static uint64_t hash(const uint64_t * const data,
                         const size_t n);

    // hash a string
    static uint64_t hash(const std::string& data);

    // hash a vector of strings
    static uint64_t hash(const std::vector< std::string >& data);
};

}; // namespace chai_internal

#endif
