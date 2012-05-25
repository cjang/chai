/*
Copyright 2010-2011, D. E. Shaw Research.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions, and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions, and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of D. E. Shaw Research nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

inline uint32_t mulhilo32(uint32_t a, uint32_t b, uint32_t* hip)
{
    uint64_t product = ((uint64_t)a) * ((uint64_t)b);
    *hip = product>>32;
    return (uint32_t)product;
}

inline uint64_t mulhilo64(uint64_t a, uint64_t b, uint64_t* hip)
{
    *hip = mul_hi(a, b);
    return a*b;
}

inline struct r123array1x32 _philox2x32bumpkey(struct r123array1x32 key)
{
    key.v[0] += ((uint32_t)0x9E3779B9);
    return key;
}

inline struct r123array2x32 _philox4x32bumpkey(struct r123array2x32 key)
{
    key.v[0] += ((uint32_t)0x9E3779B9);
    key.v[1] += ((uint32_t)0xBB67AE85);
    return key;
}

inline struct r123array2x32 _philox2x32round(struct r123array2x32 ctr, struct r123array1x32 key) __attribute__((always_inline));
inline struct r123array2x32 _philox2x32round(struct r123array2x32 ctr, struct r123array1x32 key)
{
    uint32_t hi;
    uint32_t lo = mulhilo32(((uint32_t)0xd256d193), ctr.v[0], &hi);
    struct r123array2x32 out = { {hi ^ key.v[0] ^ ctr.v[1], lo} };
    return out;
}

inline struct r123array4x32 _philox4x32round(struct r123array4x32 ctr, struct r123array2x32 key) __attribute__((always_inline));
inline struct r123array4x32 _philox4x32round(struct r123array4x32 ctr, struct r123array2x32 key)
{
    uint32_t hi0;
    uint32_t hi1;
    uint32_t lo0 = mulhilo32(((uint32_t)0xD2511F53), ctr.v[0], &hi0);
    uint32_t lo1 = mulhilo32(((uint32_t)0xCD9E8D57), ctr.v[2], &hi1);
    struct r123array4x32 out = { {hi1 ^ ctr.v[1] ^ key.v[0], lo1, hi0 ^ ctr.v[3] ^ key.v[1], lo0} };
    return out;
}

enum r123_enum_philox2x32 { philox2x32_rounds = 10 };

typedef struct r123array2x32 philox2x32_ctr_t;
typedef struct r123array1x32 philox2x32_key_t;
typedef struct r123array1x32 philox2x32_ukey_t;

inline philox2x32_key_t philox2x32keyinit(philox2x32_ukey_t uk) { return uk; }

inline philox2x32_ctr_t philox2x32_R(unsigned int R, philox2x32_ctr_t ctr, philox2x32_key_t key) __attribute__((always_inline));
inline philox2x32_ctr_t philox2x32_R(unsigned int R, philox2x32_ctr_t ctr, philox2x32_key_t key)
{
    if (R > 0) { ctr = _philox2x32round(ctr, key); }
    if (R > 1) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 2) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 3) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 4) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 5) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 6) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 7) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 8) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 9) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 10) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 11) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 12) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 13) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 14) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    if (R > 15) { key = _philox2x32bumpkey(key); ctr = _philox2x32round(ctr, key); }
    return ctr;
}

enum r123_enum_philox4x32 { philox4x32_rounds = 10 };

typedef struct r123array4x32 philox4x32_ctr_t;
typedef struct r123array2x32 philox4x32_key_t;
typedef struct r123array2x32 philox4x32_ukey_t;

inline philox4x32_key_t philox4x32keyinit(philox4x32_ukey_t uk) { return uk; }

inline philox4x32_ctr_t philox4x32_R(unsigned int R, philox4x32_ctr_t ctr, philox4x32_key_t key) __attribute__((always_inline));
inline philox4x32_ctr_t philox4x32_R(unsigned int R, philox4x32_ctr_t ctr, philox4x32_key_t key)
{
    if (R > 0) { ctr = _philox4x32round(ctr, key); }
    if (R > 1) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 2) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 3) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 4) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 5) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 6) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 7) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 8) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 9) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 10) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 11) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 12) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 13) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 14) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    if (R > 15) { key = _philox4x32bumpkey(key); ctr = _philox4x32round(ctr, key); }
    return ctr;
}

inline struct r123array1x64 _philox2x64bumpkey(struct r123array1x64 key)
{
    key.v[0] += ((ulong)(0x9E3779B97F4A7C15UL));
    return key;
}

inline struct r123array2x64 _philox4x64bumpkey(struct r123array2x64 key)
{
    key.v[0] += ((ulong)(0x9E3779B97F4A7C15UL));
    key.v[1] += ((ulong)(0xBB67AE8584CAA73BUL));
    return key;
}

inline struct r123array2x64 _philox2x64round(struct r123array2x64 ctr, struct r123array1x64 key) __attribute__((always_inline));
inline struct r123array2x64 _philox2x64round(struct r123array2x64 ctr, struct r123array1x64 key)
{
    uint64_t hi;
    uint64_t lo = mulhilo64(((ulong)(0xD2B74407B1CE6E93UL)), ctr.v[0], &hi);
    struct r123array2x64 out = { {hi ^ key.v[0] ^ ctr.v[1], lo} };
    return out;
}

inline struct r123array4x64 _philox4x64round(struct r123array4x64 ctr, struct r123array2x64 key) __attribute__((always_inline));
inline struct r123array4x64 _philox4x64round(struct r123array4x64 ctr, struct r123array2x64 key)
{
    uint64_t hi0;
    uint64_t hi1;
    uint64_t lo0 = mulhilo64(((ulong)(0xD2E7470EE14C6C93UL)), ctr.v[0], &hi0);
    uint64_t lo1 = mulhilo64(((ulong)(0xCA5A826395121157UL)), ctr.v[2], &hi1);
    struct r123array4x64 out = { {hi1 ^ ctr.v[1] ^ key.v[0], lo1, hi0 ^ ctr.v[3] ^ key.v[1], lo0} };
    return out;
}

enum r123_enum_philox2x64 { philox2x64_rounds = 10 };

typedef struct r123array2x64 philox2x64_ctr_t;
typedef struct r123array1x64 philox2x64_key_t;
typedef struct r123array1x64 philox2x64_ukey_t;

inline philox2x64_key_t philox2x64keyinit(philox2x64_ukey_t uk) { return uk; }

inline philox2x64_ctr_t philox2x64_R(unsigned int R, philox2x64_ctr_t ctr, philox2x64_key_t key) __attribute__((always_inline));
inline philox2x64_ctr_t philox2x64_R(unsigned int R, philox2x64_ctr_t ctr, philox2x64_key_t key)
{
    if (R > 0) { ctr = _philox2x64round(ctr, key); }
    if (R > 1) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 2) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 3) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 4) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 5) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 6) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 7) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 8) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 9) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 10) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 11) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 12) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 13) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 14) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    if (R > 15) { key = _philox2x64bumpkey(key); ctr = _philox2x64round(ctr, key); }
    return ctr;
}

enum r123_enum_philox4x64 { philox4x64_rounds = 10 };

typedef struct r123array4x64 philox4x64_ctr_t;
typedef struct r123array2x64 philox4x64_key_t;
typedef struct r123array2x64 philox4x64_ukey_t;

inline philox4x64_key_t philox4x64keyinit(philox4x64_ukey_t uk) { return uk; }

inline philox4x64_ctr_t philox4x64_R(unsigned int R, philox4x64_ctr_t ctr, philox4x64_key_t key) __attribute__((always_inline));
inline philox4x64_ctr_t philox4x64_R(unsigned int R, philox4x64_ctr_t ctr, philox4x64_key_t key)
{
    if (R > 0) { ctr = _philox4x64round(ctr, key); }
    if (R > 1) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 2) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 3) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 4) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 5) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 6) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 7) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 8) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 9) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 10) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 11) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 12) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 13) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 14) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    if (R > 15) { key = _philox4x64bumpkey(key); ctr = _philox4x64round(ctr, key); }
    return ctr;
}
