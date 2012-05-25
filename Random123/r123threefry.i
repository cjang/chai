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

/* Significant parts of this file were copied from
   from:
      Skein_FinalRnd/ReferenceImplementation/skein.h
      Skein_FinalRnd/ReferenceImplementation/skein_block.c

   in http://csrc.nist.gov/groups/ST/hash/sha-3/Round3/documents/Skein_FinalRnd.zip

   This file has been modified so that it may no longer perform its originally
   intended function.  If you're looking for a Skein or Threefish source code,
   please consult the original file.

   The original file had the following header:
**************************************************************************
**
** Interface declarations and internal definitions for Skein hashing.
**
** Source code author: Doug Whiting, 2008.
**
** This algorithm and source code is released to the public domain.
**
***************************************************************************

*/

enum r123_enum_threefry64x4
{
    R_64x4_0_0=14, R_64x4_0_1=16,
    R_64x4_1_0=52, R_64x4_1_1=57,
    R_64x4_2_0=23, R_64x4_2_1=40,
    R_64x4_3_0= 5, R_64x4_3_1=37,
    R_64x4_4_0=25, R_64x4_4_1=33,
    R_64x4_5_0=46, R_64x4_5_1=12,
    R_64x4_6_0=58, R_64x4_6_1=22,
    R_64x4_7_0=32, R_64x4_7_1=32
};

enum r123_enum_threefry64x2
{
    R_64x2_0_0=16,
    R_64x2_1_0=42,
    R_64x2_2_0=12,
    R_64x2_3_0=31,
    R_64x2_4_0=16,
    R_64x2_5_0=32,
    R_64x2_6_0=24,
    R_64x2_7_0=21
};

enum r123_enum_threefry32x4
{
    R_32x4_0_0=10, R_32x4_0_1=26,
    R_32x4_1_0=11, R_32x4_1_1=21,
    R_32x4_2_0=13, R_32x4_2_1=27,
    R_32x4_3_0=23, R_32x4_3_1= 5,
    R_32x4_4_0= 6, R_32x4_4_1=20,
    R_32x4_5_0=17, R_32x4_5_1=11,
    R_32x4_6_0=25, R_32x4_6_1=10,
    R_32x4_7_0=18, R_32x4_7_1=20
};

enum r123_enum_threefry32x2
{
    R_32x2_0_0=13,
    R_32x2_1_0=15,
    R_32x2_2_0=26,
    R_32x2_3_0= 6,
    R_32x2_4_0=17,
    R_32x2_5_0=29,
    R_32x2_6_0=16,
    R_32x2_7_0=24
};

inline uint64_t RotL_64(uint64_t x, unsigned int N) __attribute__((always_inline));
inline uint64_t RotL_64(uint64_t x, unsigned int N) { return (x << (N & 63)) | (x >> ((64-N) & 63)); }

inline uint32_t RotL_32(uint32_t x, unsigned int N) __attribute__((always_inline));
inline uint32_t RotL_32(uint32_t x, unsigned int N) { return (x << (N & 31)) | (x >> ((32-N) & 31)); }

/*
 * 2x64
 */

typedef struct r123array2x64 threefry2x64_ctr_t;
typedef struct r123array2x64 threefry2x64_key_t;
typedef struct r123array2x64 threefry2x64_ukey_t;

inline threefry2x64_key_t threefry2x64keyinit(threefry2x64_ukey_t uk) { return uk; }

inline threefry2x64_ctr_t threefry2x64_R(unsigned int Nrounds, threefry2x64_ctr_t in, threefry2x64_key_t k) __attribute__((always_inline));
inline threefry2x64_ctr_t threefry2x64_R(unsigned int Nrounds, threefry2x64_ctr_t in, threefry2x64_key_t k)
{
    threefry2x64_ctr_t X;
    uint64_t ks[2+1];
    int i;

    ks[2] = ((0xA9FC1A22) + (((uint64_t) (0x1BD11BDA)) << 32));

    for (i = 0; i < 2; i++)
    {
        ks[i] = k.v[i];
        X.v[i] = in.v[i];
        ks[2] ^= k.v[i];
    }

    X.v[0] += ks[0];
    X.v[1] += ks[1];

    if (Nrounds > 0) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 1) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 2) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 3) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 3) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[1] += 1; }

    if (Nrounds > 4) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_4_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 5) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_5_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 6) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_6_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 7) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_7_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 7) { X.v[0] += ks[2]; X.v[1] += ks[0]; X.v[1] += 2; }

    if (Nrounds > 8) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 9) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 10) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 11) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 11) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[1] += 3; }

    if (Nrounds > 12) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_4_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 13) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_5_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 14) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_6_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 15) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_7_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 15) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[1] += 4; }

    if (Nrounds > 16) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 17) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 18) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 19) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 19) { X.v[0] += ks[2]; X.v[1] += ks[0]; X.v[1] += 5; }

    if (Nrounds > 20) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 21) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 22) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 23) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 23) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[1] += 6; }

    if (Nrounds > 24) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_4_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 25) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_5_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 26) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_6_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 27) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_7_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 27) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[1] += 7; }

    if (Nrounds > 28) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 29) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 30) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 31) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 31) { X.v[0] += ks[2]; X.v[1] += ks[0]; X.v[1] += 8; }

    return X;
}

enum r123_enum_threefry2x64 { threefry2x64_rounds = 20 };

inline threefry2x64_ctr_t threefry2x64(threefry2x64_ctr_t in, threefry2x64_key_t k) __attribute__((always_inline));
inline threefry2x64_ctr_t threefry2x64(threefry2x64_ctr_t in, threefry2x64_key_t k) { return threefry2x64_R(threefry2x64_rounds, in, k); }

/*
 * 2x32
 */

typedef struct r123array2x32 threefry2x32_ctr_t;
typedef struct r123array2x32 threefry2x32_key_t;
typedef struct r123array2x32 threefry2x32_ukey_t;

inline threefry2x32_key_t threefry2x32keyinit(threefry2x32_ukey_t uk) { return uk; }

inline threefry2x32_ctr_t threefry2x32_R(unsigned int Nrounds, threefry2x32_ctr_t in, threefry2x32_key_t k) __attribute__((always_inline));
inline threefry2x32_ctr_t threefry2x32_R(unsigned int Nrounds, threefry2x32_ctr_t in, threefry2x32_key_t k)
{
    threefry2x32_ctr_t X;
    uint32_t ks[2+1];
    int i;

    ks[2] = 0x1BD11BDA;

    for (i = 0; i < 2; i++)
    {
        ks[i] = k.v[i];
        X.v[i] = in.v[i];
        ks[2] ^= k.v[i];
    }

    X.v[0] += ks[0];
    X.v[1] += ks[1];

    if (Nrounds > 0) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 1) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 2) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 3) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 3) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[1] += 1; }

    if (Nrounds > 4) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_4_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 5) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_5_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 6) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_6_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 7) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_7_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 7) { X.v[0] += ks[2]; X.v[1] += ks[0]; X.v[1] += 2; }

    if (Nrounds > 8) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 9) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 10) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 11) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 11) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[1] += 3; }

    if (Nrounds > 12) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_4_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 13) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_5_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 14) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_6_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 15) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_7_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 15) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[1] += 4; }

    if (Nrounds > 16) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 17) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 18) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 19) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 19) { X.v[0] += ks[2]; X.v[1] += ks[0]; X.v[1] += 5; }

    if (Nrounds > 20) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 21) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 22) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 23) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 23) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[1] += 6; }

    if (Nrounds > 24) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_4_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 25) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_5_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 26) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_6_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 27) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_7_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 27) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[1] += 7; }

    if (Nrounds > 28) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_0_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 29) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_1_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 30) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_2_0); X.v[1] ^= X.v[0]; }
    if (Nrounds > 31) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x2_3_0); X.v[1] ^= X.v[0]; }

    if (Nrounds > 31) { X.v[0] += ks[2]; X.v[1] += ks[0]; X.v[1] += 8; }

    return X;
}

enum r123_enum_threefry2x32 { threefry2x32_rounds = 20 };

inline threefry2x32_ctr_t threefry2x32(threefry2x32_ctr_t in, threefry2x32_key_t k) __attribute__((always_inline));
inline threefry2x32_ctr_t threefry2x32(threefry2x32_ctr_t in, threefry2x32_key_t k) { return threefry2x32_R(threefry2x32_rounds, in, k); }

/*
 * 4x64
 */

typedef struct r123array4x64 threefry4x64_ctr_t;
typedef struct r123array4x64 threefry4x64_key_t;
typedef struct r123array4x64 threefry4x64_ukey_t;

inline threefry4x64_key_t threefry4x64keyinit(threefry4x64_ukey_t uk) { return uk; }

inline threefry4x64_ctr_t threefry4x64_R(unsigned int Nrounds, threefry4x64_ctr_t in, threefry4x64_key_t k) __attribute__((always_inline));
inline threefry4x64_ctr_t threefry4x64_R(unsigned int Nrounds, threefry4x64_ctr_t in, threefry4x64_key_t k)
{
    threefry4x64_ctr_t X;
    uint64_t ks[4+1];
    int i;

    ks[4] = ((0xA9FC1A22) + (((uint64_t) (0x1BD11BDA)) << 32));

    for (i = 0; i < 4; i++)
    {
        ks[i] = k.v[i];
        X.v[i] = in.v[i];
        ks[4] ^= k.v[i];
    }

    X.v[0] += ks[0];
    X.v[1] += ks[1];
    X.v[2] += ks[2];
    X.v[3] += ks[3];

    if (Nrounds > 0) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 1) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 2) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 3) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 3) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[2] += ks[3]; X.v[3] += ks[4]; X.v[4-1] += 1; }

    if (Nrounds > 4) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 5) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 6) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 7) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 7) { X.v[0] += ks[2]; X.v[1] += ks[3]; X.v[2] += ks[4]; X.v[3] += ks[0]; X.v[4-1] += 2; }

    if (Nrounds > 8) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 9) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 10) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 11) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 11) { X.v[0] += ks[3]; X.v[1] += ks[4]; X.v[2] += ks[0]; X.v[3] += ks[1]; X.v[4-1] += 3; }

    if (Nrounds > 12) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 13) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 14) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 15) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 15) { X.v[0] += ks[4]; X.v[1] += ks[0]; X.v[2] += ks[1]; X.v[3] += ks[2]; X.v[4-1] += 4; }

    if (Nrounds > 16) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 17) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 18) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 19) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 19) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[2] += ks[2]; X.v[3] += ks[3]; X.v[4-1] += 5; }

    if (Nrounds > 20) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 21) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 22) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 23) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 23) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[2] += ks[3]; X.v[3] += ks[4]; X.v[4-1] += 6; }

    if (Nrounds > 24) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 25) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 26) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 27) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 27) { X.v[0] += ks[2]; X.v[1] += ks[3]; X.v[2] += ks[4]; X.v[3] += ks[0]; X.v[4-1] += 7; }

    if (Nrounds > 28) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 29) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 30) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 31) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 31) { X.v[0] += ks[3]; X.v[1] += ks[4]; X.v[2] += ks[0]; X.v[3] += ks[1]; X.v[4-1] += 8; }

    if (Nrounds > 32) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 33) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 34) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 35) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 35) { X.v[0] += ks[4]; X.v[1] += ks[0]; X.v[2] += ks[1]; X.v[3] += ks[2]; X.v[4-1] += 9; }

    if (Nrounds > 36) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 37) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 38) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 39) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 39) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[2] += ks[2]; X.v[3] += ks[3]; X.v[4-1] += 10; }

    if (Nrounds > 40) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 41) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 42) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 43) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 43) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[2] += ks[3]; X.v[3] += ks[4]; X.v[4-1] += 11; }

    if (Nrounds > 44) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 45) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 46) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 47) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 47) { X.v[0] += ks[2]; X.v[1] += ks[3]; X.v[2] += ks[4]; X.v[3] += ks[0]; X.v[4-1] += 12; }

    if (Nrounds > 48) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 49) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 50) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 51) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 51) { X.v[0] += ks[3]; X.v[1] += ks[4]; X.v[2] += ks[0]; X.v[3] += ks[1]; X.v[4-1] += 13; }

    if (Nrounds > 52) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 53) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 54) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 55) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 55) { X.v[0] += ks[4]; X.v[1] += ks[0]; X.v[2] += ks[1]; X.v[3] += ks[2]; X.v[4-1] += 14; }

    if (Nrounds > 56) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 57) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 58) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 59) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 59) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[2] += ks[2]; X.v[3] += ks[3]; X.v[4-1] += 15; }

    if (Nrounds > 60) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 61) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 62) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 63) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 63) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[2] += ks[3]; X.v[3] += ks[4]; X.v[4-1] += 16; }

    if (Nrounds > 64) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 65) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 66) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 67) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 67) { X.v[0] += ks[2]; X.v[1] += ks[3]; X.v[2] += ks[4]; X.v[3] += ks[0]; X.v[4-1] += 17; }

    if (Nrounds > 68) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 69) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 70) { X.v[0] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 71) { X.v[0] += X.v[3]; X.v[3] = RotL_64(X.v[3], R_64x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_64(X.v[1], R_64x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 71) { X.v[0] += ks[3]; X.v[1] += ks[4]; X.v[2] += ks[0]; X.v[3] += ks[1]; X.v[4-1] += 18; }

    return X;
}

enum r123_enum_threefry4x64 { threefry4x64_rounds = 20 };

inline threefry4x64_ctr_t threefry4x64(threefry4x64_ctr_t in, threefry4x64_key_t k) __attribute__((always_inline));
inline threefry4x64_ctr_t threefry4x64(threefry4x64_ctr_t in, threefry4x64_key_t k) { return threefry4x64_R(threefry4x64_rounds, in, k); }

/*
 * 4x32
 */

typedef struct r123array4x32 threefry4x32_ctr_t;
typedef struct r123array4x32 threefry4x32_key_t;
typedef struct r123array4x32 threefry4x32_ukey_t;

inline threefry4x32_key_t threefry4x32keyinit(threefry4x32_ukey_t uk) { return uk; }

inline threefry4x32_ctr_t threefry4x32_R(unsigned int Nrounds, threefry4x32_ctr_t in, threefry4x32_key_t k) __attribute__((always_inline));
inline threefry4x32_ctr_t threefry4x32_R(unsigned int Nrounds, threefry4x32_ctr_t in, threefry4x32_key_t k)
{
    threefry4x32_ctr_t X;
    uint32_t ks[4+1];
    int i;

    ks[4] = 0x1BD11BDA;

    for (i = 0; i < 4; i++)
    {
        ks[i] = k.v[i];
        X.v[i] = in.v[i];
        ks[4] ^= k.v[i];
    }

    X.v[0] += ks[0];
    X.v[1] += ks[1];
    X.v[2] += ks[2];
    X.v[3] += ks[3];

    if (Nrounds > 0) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 1) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 2) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 3) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 3) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[2] += ks[3]; X.v[3] += ks[4]; X.v[4-1] += 1; }

    if (Nrounds > 4) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 5) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 6) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 7) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 7) { X.v[0] += ks[2]; X.v[1] += ks[3]; X.v[2] += ks[4]; X.v[3] += ks[0]; X.v[4-1] += 2; }

    if (Nrounds > 8) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 9) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 10) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 11) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 11) { X.v[0] += ks[3]; X.v[1] += ks[4]; X.v[2] += ks[0]; X.v[3] += ks[1]; X.v[4-1] += 3; }

    if (Nrounds > 12) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 13) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 14) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 15) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 15) { X.v[0] += ks[4]; X.v[1] += ks[0]; X.v[2] += ks[1]; X.v[3] += ks[2]; X.v[4-1] += 4; }

    if (Nrounds > 16) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 17) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 18) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 19) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 19) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[2] += ks[2]; X.v[3] += ks[3]; X.v[4-1] += 5; }

    if (Nrounds > 20) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 21) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 22) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 23) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 23) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[2] += ks[3]; X.v[3] += ks[4]; X.v[4-1] += 6; }

    if (Nrounds > 24) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 25) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 26) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 27) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 27) { X.v[0] += ks[2]; X.v[1] += ks[3]; X.v[2] += ks[4]; X.v[3] += ks[0]; X.v[4-1] += 7; }

    if (Nrounds > 28) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 29) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 30) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 31) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 31) { X.v[0] += ks[3]; X.v[1] += ks[4]; X.v[2] += ks[0]; X.v[3] += ks[1]; X.v[4-1] += 8; }

    if (Nrounds > 32) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 33) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 34) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 35) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 35) { X.v[0] += ks[4]; X.v[1] += ks[0]; X.v[2] += ks[1]; X.v[3] += ks[2]; X.v[4-1] += 9; }

    if (Nrounds > 36) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 37) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 38) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 39) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 39) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[2] += ks[2]; X.v[3] += ks[3]; X.v[4-1] += 10; }

    if (Nrounds > 40) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 41) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 42) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 43) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 43) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[2] += ks[3]; X.v[3] += ks[4]; X.v[4-1] += 11; }

    if (Nrounds > 44) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 45) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 46) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 47) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 47) { X.v[0] += ks[2]; X.v[1] += ks[3]; X.v[2] += ks[4]; X.v[3] += ks[0]; X.v[4-1] += 12; }

    if (Nrounds > 48) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 49) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 50) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 51) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 51) { X.v[0] += ks[3]; X.v[1] += ks[4]; X.v[2] += ks[0]; X.v[3] += ks[1]; X.v[4-1] += 13; }

    if (Nrounds > 52) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 53) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 54) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 55) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 55) { X.v[0] += ks[4]; X.v[1] += ks[0]; X.v[2] += ks[1]; X.v[3] += ks[2]; X.v[4-1] += 14; }

    if (Nrounds > 56) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 57) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 58) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 59) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 59) { X.v[0] += ks[0]; X.v[1] += ks[1]; X.v[2] += ks[2]; X.v[3] += ks[3]; X.v[4-1] += 15; }

    if (Nrounds > 60) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 61) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 62) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 63) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 63) { X.v[0] += ks[1]; X.v[1] += ks[2]; X.v[2] += ks[3]; X.v[3] += ks[4]; X.v[4-1] += 16; }

    if (Nrounds > 64) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_0_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_0_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 65) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_1_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_1_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 66) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_2_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_2_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 67) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_3_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_3_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 67) { X.v[0] += ks[2]; X.v[1] += ks[3]; X.v[2] += ks[4]; X.v[3] += ks[0]; X.v[4-1] += 17; }

    if (Nrounds > 68) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_4_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_4_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 69) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_5_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_5_1); X.v[1] ^= X.v[2]; }
    if (Nrounds > 70) { X.v[0] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_6_0); X.v[1] ^= X.v[0]; X.v[2] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_6_1); X.v[3] ^= X.v[2]; }
    if (Nrounds > 71) { X.v[0] += X.v[3]; X.v[3] = RotL_32(X.v[3], R_32x4_7_0); X.v[3] ^= X.v[0]; X.v[2] += X.v[1]; X.v[1] = RotL_32(X.v[1], R_32x4_7_1); X.v[1] ^= X.v[2]; }

    if (Nrounds > 71) { X.v[0] += ks[3]; X.v[1] += ks[4]; X.v[2] += ks[0]; X.v[3] += ks[1]; X.v[4-1] += 18; }

    return X;
}

enum r123_enum_threefry4x32 { threefry4x32_rounds = 20 };

inline threefry4x32_ctr_t threefry4x32(threefry4x32_ctr_t in, threefry4x32_key_t k) __attribute__((always_inline));
inline threefry4x32_ctr_t threefry4x32(threefry4x32_ctr_t in, threefry4x32_key_t k) { return threefry4x32_R(threefry4x32_rounds, in, k); }
