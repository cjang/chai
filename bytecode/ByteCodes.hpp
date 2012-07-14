// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BYTE_CODES_HPP_
#define _CHAI_BYTE_CODES_HPP_

#include <stdint.h>

namespace chai_internal {

////////////////////////////////////////
// operation bytecodes

namespace ByteCodes {

    // no particular reason to start operation codes at 1, just seems nice
    // to leave 0 unassigned - it may be useful later as a NULL flag

    ////////////////////////////////////////
    // internal (1 - 9)

    static const uint32_t convert_u32             = 1;
    static const uint32_t convert_i32             = 2;
    static const uint32_t convert_f32             = 3;
    static const uint32_t convert_f64             = 4;
    static const uint32_t scalar_u32              = 5;
    static const uint32_t scalar_i32              = 6;
    static const uint32_t scalar_f32              = 7;
    static const uint32_t scalar_f64              = 8;

    ////////////////////////////////////////
    // arithmetic operators (10 - 29)

    static const uint32_t operatorUNARYMINUS      = 10;
    static const uint32_t operatorADD             = 11;
    static const uint32_t operatorDIVIDE          = 12;
    static const uint32_t operatorMULTIPLY        = 13;
    static const uint32_t operatorSUBTRACT        = 14;
    static const uint32_t operatorCOMPLEMENT      = 15;
    static const uint32_t operatorNOT             = 16;
    static const uint32_t operatorMODULO          = 17;
    static const uint32_t operatorSHIFTLEFT       = 18;
    static const uint32_t operatorSHIFTRIGHT      = 19;
    static const uint32_t operatorBITWISEAND      = 20;
    static const uint32_t operatorBITWISEOR       = 21;
    static const uint32_t operatorBITWISEXOR      = 22;
    static const uint32_t operatorCONDEXPR        = 23;

    ////////////////////////////////////////
    // predicate operators (30 - 39)

    static const uint32_t operatorLOGICALAND      = 30;
    static const uint32_t operatorLOGICALOR       = 31;
    static const uint32_t operatorEQUAL           = 32;
    static const uint32_t operatorNOTEQUAL        = 33;
    static const uint32_t operatorGREATEREQUAL    = 34;
    static const uint32_t operatorGREATERTHAN     = 35;
    static const uint32_t operatorLESSEQUAL       = 36;
    static const uint32_t operatorLESSTHAN        = 37;

    ////////////////////////////////////////
    // gathering (40 - 49)

    static const uint32_t gather1_floor           = 40;
    static const uint32_t gather2_floor           = 41;
    static const uint32_t index1_u32              = 42;
    static const uint32_t index1_i32              = 43;
    static const uint32_t index1_f32              = 44;
    static const uint32_t index1_f64              = 45;
    static const uint32_t index2_u32              = 46;
    static const uint32_t index2_i32              = 47;
    static const uint32_t index2_f32              = 48;
    static const uint32_t index2_f64              = 49;

    ////////////////////////////////////////
    // special kernel (50 - 59)

    static const uint32_t kernel_from_opencl      = 50;

    ////////////////////////////////////////
    // data creation (60 - 89)

    static const uint32_t make1_u32               = 60;
    static const uint32_t make1_i32               = 61;
    static const uint32_t make1_f32               = 62;
    static const uint32_t make1_f64               = 63;
    static const uint32_t make2_u32               = 64;
    static const uint32_t make2_i32               = 65;
    static const uint32_t make2_f32               = 66;
    static const uint32_t make2_f64               = 67;
    static const uint32_t ones1_u32               = 68;
    static const uint32_t ones1_i32               = 69;
    static const uint32_t ones1_f32               = 70;
    static const uint32_t ones1_f64               = 71;
    static const uint32_t ones2_u32               = 72;
    static const uint32_t ones2_i32               = 73;
    static const uint32_t ones2_f32               = 74;
    static const uint32_t ones2_f64               = 75;
    static const uint32_t zeros1_u32              = 76;
    static const uint32_t zeros1_i32              = 77;
    static const uint32_t zeros1_f32              = 78;
    static const uint32_t zeros1_f64              = 79;
    static const uint32_t zeros2_u32              = 80;
    static const uint32_t zeros2_i32              = 81;
    static const uint32_t zeros2_f32              = 82;
    static const uint32_t zeros2_f64              = 73;

    ////////////////////////////////////////
    // read back data (90 - 109)

    static const uint32_t read_scalar_u32         = 90;
    static const uint32_t read_scalar_i32         = 91;
    static const uint32_t read_scalar_f32         = 92;
    static const uint32_t read_scalar_f64         = 93;
    static const uint32_t read1_u32               = 94;
    static const uint32_t read1_i32               = 95;
    static const uint32_t read1_f32               = 96;
    static const uint32_t read1_f64               = 97;
    static const uint32_t read2_u32               = 98;
    static const uint32_t read2_i32               = 99;
    static const uint32_t read2_f32               = 100;
    static const uint32_t read2_f64               = 101;

    ////////////////////////////////////////
    // random numbers (110 - 119)

    static const uint32_t rng_normal_make_f32     = 110;
    static const uint32_t rng_normal_make_f64     = 111;
    static const uint32_t rng_uniform_make_u32    = 112;
    static const uint32_t rng_uniform_make_i32    = 113;
    static const uint32_t rng_uniform_make_f32    = 114;
    static const uint32_t rng_uniform_make_f64    = 115;

    ////////////////////////////////////////
    // auto-tuned matrix multipy (120 - 129)

    static const uint32_t matmul                  = 120;
    static const uint32_t matmulG                 = 121;
    static const uint32_t transpose               = 122;

    ////////////////////////////////////////
    // reductions (130 - 139)

    static const uint32_t dot_product             = 130;
    static const uint32_t mean                    = 131;
    static const uint32_t sum                     = 132;

    ////////////////////////////////////////
    // math functions (200 - 299)

    // arc cosine
    static const uint32_t acos                    = 200;
    static const uint32_t acosh                   = 201;
    static const uint32_t acospi                  = 202;

    // arc sine
    static const uint32_t asin                    = 203;
    static const uint32_t asinh                   = 204;
    static const uint32_t asinpi                  = 205;

    // arc tangent
    static const uint32_t atan                    = 206;
    static const uint32_t atan2                   = 207;
    static const uint32_t atanh                   = 208;
    static const uint32_t atanpi                  = 209;
    static const uint32_t atan2pi                 = 210;

    // cube root
    static const uint32_t cbrt                    = 211;

    // ceiling, copysign
    static const uint32_t ceil                    = 212;
    static const uint32_t copysign                = 213;

    // cosine
    static const uint32_t cos                     = 214;
    static const uint32_t cosh                    = 215;
    static const uint32_t cospi                   = 216;

    // error function
    static const uint32_t erfc                    = 217;
    static const uint32_t erf                     = 218;

    // exponential
    static const uint32_t exp                     = 219;
    static const uint32_t exp2                    = 220;
    static const uint32_t exp10                   = 221;
    static const uint32_t expm1                   = 222;

    // absolute value
    static const uint32_t fabs                    = 223;

    // positive difference
    static const uint32_t fdim                    = 224;

    // floor
    static const uint32_t floor                   = 225;

    // fused multiply add
    static const uint32_t fma                     = 226;

    // maximum, minimum
    static const uint32_t fmax                    = 227;
    static const uint32_t fmin                    = 228;

    // remainder
    static const uint32_t fmod                    = 229;

    // fractional parts
    //static const uint32_t fract                   = 230; // unimplemented
    //static const uint32_t frexp                   = 231; // unimplemented

    // Euclidean distance
    static const uint32_t hypot                   = 232;

    // exponent
    static const uint32_t ilogb                   = 233;

    // multiply by power of 2
    static const uint32_t ldexp                   = 234;

    // log gamma function
    static const uint32_t lgamma                  = 235;
    //static const uint32_t lgamma_r                = 236; // unimplemented

    // logarithm
    static const uint32_t log                     = 237;
    static const uint32_t log2                    = 238;
    static const uint32_t log10                   = 239;
    static const uint32_t log1p                   = 240;
    static const uint32_t logb                    = 241;

    // fast multiply add
    static const uint32_t mad                     = 242;

    // magnitude
    static const uint32_t maxmag                  = 243;
    static const uint32_t minmag                  = 244;

    // decompose floating point number
    //static const uint32_t modf                    = 245; // unimplemented

    // not a number
    static const uint32_t nan                     = 246;

    // next representable floating point number
    static const uint32_t nextafter               = 247;

    // raise to power
    static const uint32_t pow                     = 248;
    static const uint32_t pown                    = 249;
    static const uint32_t powr                    = 250;

    // remainder and quotient
    static const uint32_t remainder               = 251;
    //static const uint32_t remquo                  = 252; // unimplemented

    // round to integral value
    static const uint32_t rint                    = 253;

    // n-th root
    static const uint32_t rootn                   = 254;

    // round away from zero
    static const uint32_t round                   = 255;

    // inverse square root
    static const uint32_t rsqrt                   = 256;

    // sine
    static const uint32_t sin                     = 257;
    //static const uint32_t sincos                  = 258; // unimplemented
    static const uint32_t sinh                    = 259;
    static const uint32_t sinpi                   = 260;

    // square root
    static const uint32_t sqrt                    = 261;

    // tangent
    static const uint32_t tan                     = 262;
    static const uint32_t tanh                    = 263;
    static const uint32_t tanpi                   = 264;

    // gamma function
    static const uint32_t tgamma                  = 265;

    // truncate
    static const uint32_t trunc                   = 266;

    ////////////////////////////////////////
    // common functions (300 - 399)

    static const uint32_t clamp                   = 300;
    static const uint32_t degrees                 = 301;
    static const uint32_t max                     = 302;
    static const uint32_t min                     = 303;
    static const uint32_t mix                     = 304;
    static const uint32_t radians                 = 305;
    static const uint32_t step                    = 306;
    static const uint32_t smoothstep              = 307;
    static const uint32_t sign                    = 308;

    ////////////////////////////////////////
    // integer functions (400 - 499)

    static const uint32_t abs                     = 400;
    static const uint32_t abs_diff                = 401;
    static const uint32_t add_sat                 = 402;
    //static const uint32_t clamp                   = 403;
    static const uint32_t clz                     = 404;
    static const uint32_t hadd                    = 405;
    static const uint32_t mad24                   = 406;
    static const uint32_t mad_hi                  = 407;
    static const uint32_t mad_sat                 = 408;
    //static const uint32_t max                     = 409;
    //static const uint32_t min                     = 410;
    static const uint32_t mul24                   = 411;
    static const uint32_t mul_hi                  = 412;
    static const uint32_t rhadd                   = 413;
    static const uint32_t rotate                  = 414;
    static const uint32_t sub_sat                 = 415;

    ////////////////////////////////////////
    // relational functions (500 - 599)

    static const uint32_t isequal                 = 500;
    static const uint32_t isnotequal              = 501;
    static const uint32_t isgreater               = 502;
    static const uint32_t isgreaterequal          = 503;
    static const uint32_t isless                  = 504;
    static const uint32_t islessequal             = 505;
    static const uint32_t islessgreater           = 506;
    static const uint32_t isfinite                = 507;
    static const uint32_t isinf                   = 508;
    static const uint32_t isnan                   = 509;
    static const uint32_t isnormal                = 510;
    static const uint32_t isordered               = 511;
    static const uint32_t isunordered             = 512;
    static const uint32_t signbit                 = 513;

}; // namespace ByteCodes

}; // namespace chai_internal

#endif
