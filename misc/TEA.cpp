// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "TEA.hpp"

namespace chai_internal {

// David Wagner's new variant implementation from
// http://143.53.36.235:8080/source.htm#new_ansi
void TEA::encipher(const uint32_t * const v,
                   uint32_t * const w,
                   const uint32_t * const k)
{
    register uint32_t
        y = v[0],
        z = v[1],
        sum = 0,
        delta = 0x9E3779B9,
        n = 32;

    while (n-- > 0)
    {
        y += (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
        sum += delta;
        z += (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
    }

    w[0] = y;
    w[1] = z;
}

// David Wagner's new variant implementation from
// http://143.53.36.235:8080/source.htm#new_ansi
void TEA::decipher(const uint32_t * const v,
                   uint32_t * const w,
                   const uint32_t * const k)
{
    register uint32_t
        y = v[0],
        z = v[1],
        sum = 0xC6EF3720,
        delta = 0x9E3779B9,
        n=32;

    /* sum = delta<<5, in general sum = delta * n */

    while (n-- > 0)
    {
        z -= (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
        sum -= delta;
        y -= (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
    }
   
    w[0] = y;
    w[1] = z;
}

// Davies-Meyer hash function
uint64_t TEA::hash(const uint64_t * const data,
                   const size_t n)
{
    uint32_t v[2];
    v[0] = 314159; // arbitrary initial magic seed number
    v[1] = 271828; // arbitrary initial magic seed number

    uint32_t k[4];
    k[1] = 951413; // arbitrary key
    k[0] = 828172; // arbitrary key

    uint32_t w[2];
    w[0] = w[1] = 0;
    size_t index = 0;
    const uint32_t * pData = reinterpret_cast< const uint32_t* >(data);

    for (size_t i = 0; i < n; i++) {
        k[1] = v[0];
        k[0] = v[1];
        k[3] = pData[index++];
        k[2] = pData[index++];
        encipher(v, w, k);
        v[0] = w[0];
        v[1] = w[1];
    }

    return *reinterpret_cast< const uint64_t* >(w);
}

}; // namespace chai_internal