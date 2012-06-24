// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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
    static
    void encipher(const uint32_t * const v,
                  uint32_t * const w,
                  const uint32_t * const k);

    static
    void decipher(const uint32_t * const v,
                  uint32_t * const w,
                  const uint32_t * const k);

    static
    uint64_t hash(const uint64_t * const data,
                  const size_t n);

    static
    uint64_t hash(const std::string& data);

    static
    uint64_t hash(const std::vector< std::string >& data);
};

}; // namespace chai_internal

#endif
