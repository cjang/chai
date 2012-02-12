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

    static const uint32_t convert_f32             = 1;
    static const uint32_t convert_f64             = 2;
    static const uint32_t scalar_f32              = 3;
    static const uint32_t scalar_f64              = 4;

    ////////////////////////////////////////
    // arithmetic and predicate operators (10 - 29)

    static const uint32_t negate                  = 10;
    static const uint32_t operatorADD             = 11;
    static const uint32_t operatorAND             = 12;
    static const uint32_t operatorDIV             = 13;
    static const uint32_t operatorMUL             = 14;
    static const uint32_t operatorOR              = 15;
    static const uint32_t operatorSUB             = 16;
    static const uint32_t operatorEQ              = 17;
    static const uint32_t operatorGE              = 18;
    static const uint32_t operatorGT              = 19;
    static const uint32_t operatorLE              = 20;
    static const uint32_t operatorLT              = 21;
    static const uint32_t operatorNE              = 22;

    ////////////////////////////////////////
    // selection and gathering (30 - 39)

    static const uint32_t cond                    = 30;
    static const uint32_t gather1_floor           = 31;
    static const uint32_t gather2_floor           = 32;
    static const uint32_t index1_f32              = 33;
    static const uint32_t index1_f64              = 34;
    static const uint32_t index2_f32              = 35;
    static const uint32_t index2_f64              = 36;

    ////////////////////////////////////////
    // data creation (40 - 59)

    static const uint32_t make1_f32               = 40;
    static const uint32_t make1_f64               = 41;
    static const uint32_t make2_f32               = 42;
    static const uint32_t make2_f64               = 43;
    static const uint32_t ones1_f32               = 44;
    static const uint32_t ones1_f64               = 45;
    static const uint32_t ones2_f32               = 46;
    static const uint32_t ones2_f64               = 47;
    static const uint32_t zeros1_f32              = 48;
    static const uint32_t zeros1_f64              = 49;
    static const uint32_t zeros2_f32              = 50;
    static const uint32_t zeros2_f64              = 51;

    ////////////////////////////////////////
    // read back data (60 - 69)

    static const uint32_t read_scalar_f32         = 60;
    static const uint32_t read_scalar_f64         = 61;
    static const uint32_t read1_f32               = 62;
    static const uint32_t read1_f64               = 63;
    static const uint32_t read2_f32               = 64;
    static const uint32_t read2_f64               = 65;

    ////////////////////////////////////////
    // random numbers (70 - 79)

    static const uint32_t rng_normal_make_f32     = 70;
    static const uint32_t rng_normal_make_f64     = 71;
    static const uint32_t rng_uniform_make_f32    = 72;
    static const uint32_t rng_uniform_make_f64    = 73;

    ////////////////////////////////////////
    // auto-tuned matrix multipy (80 - 89)

    static const uint32_t matmul                  = 80;
    static const uint32_t matmulG                 = 81;
    static const uint32_t transpose               = 82;

    ////////////////////////////////////////
    // reductions (90 - 99)

    static const uint32_t dot_product             = 90;
    static const uint32_t mean                    = 91;
    static const uint32_t sum                     = 92;

    ////////////////////////////////////////
    // math functions (100 - 199)

    // arc cosine
    static const uint32_t acos                    = 100;
    static const uint32_t acosh                   = 101;
    static const uint32_t acospi                  = 102;

    // arc sine
    static const uint32_t asin                    = 103;
    static const uint32_t asinh                   = 104;
    static const uint32_t asinpi                  = 105;

    // arc tangent
    static const uint32_t atan                    = 106;
    static const uint32_t atan2                   = 107;
    static const uint32_t atanh                   = 108;
    static const uint32_t atanpi                  = 109;
    static const uint32_t atan2pi                 = 110;

    // cube root
    static const uint32_t cbrt                    = 111;

    // ceiling, copysign
    static const uint32_t ceil                    = 112;
    static const uint32_t copysign                = 113;

    // cosine
    static const uint32_t cos                     = 114;
    static const uint32_t cosh                    = 115;
    static const uint32_t cospi                   = 116;

    // error function
    static const uint32_t erfc                    = 117;
    static const uint32_t erf                     = 118;

    // exponential
    static const uint32_t exp                     = 119;
    static const uint32_t exp2                    = 120;
    static const uint32_t exp10                   = 121;
    static const uint32_t expm1                   = 122;

    // absolute value
    static const uint32_t fabs                    = 123;

    // positive difference
    static const uint32_t fdim                    = 124;

    // floor
    static const uint32_t floor                   = 125;

    // fused multiply add
    static const uint32_t fma                     = 126;

    // maximum, minimum
    static const uint32_t fmax                    = 127;
    static const uint32_t fmin                    = 128;

    // remainder
    static const uint32_t fmod                    = 129;

    // fractional parts
    //static const uint32_t fract                   = 130; // unimplemented
    //static const uint32_t frexp                   = 131; // unimplemented

    // Euclidean distance
    static const uint32_t hypot                   = 132;

    // exponent
    static const uint32_t ilogb                   = 133;

    // multiply by power of 2
    static const uint32_t ldexp                   = 134;

    // log gamma function
    static const uint32_t lgamma                  = 135;
    //static const uint32_t lgamma_r                = 136; // unimplemented

    // logarithm
    static const uint32_t log                     = 137;
    static const uint32_t log2                    = 138;
    static const uint32_t log10                   = 139;
    static const uint32_t log1p                   = 140;
    static const uint32_t logb                    = 141;

    // fast multiply add
    static const uint32_t mad                     = 142;

    // magnitude
    static const uint32_t maxmag                  = 143;
    static const uint32_t minmag                  = 144;

    // decompose floating point number
    //static const uint32_t modf                    = 145; // unimplemented

    // not a number
    static const uint32_t nan                     = 146;

    // next representable floating point number
    static const uint32_t nextafter               = 147;

    // raise to power
    static const uint32_t pow                     = 148;
    static const uint32_t pown                    = 149;
    static const uint32_t powr                    = 150;

    // remainder and quotient
    static const uint32_t remainder               = 151;
    //static const uint32_t remquo                  = 152; // unimplemented

    // round to integral value
    static const uint32_t rint                    = 153;

    // n-th root
    static const uint32_t rootn                   = 154;

    // round away from zero
    static const uint32_t round                   = 155;

    // inverse square root
    static const uint32_t rsqrt                   = 156;

    // sine
    static const uint32_t sin                     = 157;
    //static const uint32_t sincos                  = 158; // unimplemented
    static const uint32_t sinh                    = 159;
    static const uint32_t sinpi                   = 160;

    // square root
    static const uint32_t sqrt                    = 161;

    // tangent
    static const uint32_t tan                     = 162;
    static const uint32_t tanh                    = 163;
    static const uint32_t tanpi                   = 164;

    // gamma function
    static const uint32_t tgamma                  = 165;

    // truncate
    static const uint32_t trunc                   = 166;

    ////////////////////////////////////////
    // common functions (200 - 299)

    static const uint32_t clamp                   = 200;
    static const uint32_t degrees                 = 201;
    static const uint32_t max                     = 202;
    static const uint32_t min                     = 203;
    static const uint32_t mix                     = 204;
    static const uint32_t radians                 = 205;
    static const uint32_t step                    = 206;
    static const uint32_t smoothstep              = 207;
    static const uint32_t sign                    = 208;

    ////////////////////////////////////////
    // relational functions (400 - 499)

    static const uint32_t isequal                 = 400;
    static const uint32_t isnotequal              = 401;
    static const uint32_t isgreater               = 402;
    static const uint32_t isgreaterequal          = 403;
    static const uint32_t isless                  = 404;
    static const uint32_t islessequal             = 405;
    static const uint32_t islessgreater           = 406;
    static const uint32_t isfinite                = 407;
    static const uint32_t isinf                   = 408;
    static const uint32_t isnan                   = 409;
    static const uint32_t isnormal                = 410;
    static const uint32_t isordered               = 411;
    static const uint32_t isunordered             = 412;
    static const uint32_t signbit                 = 413;

}; // namespace ByteCodes

}; // namespace chai_internal

#endif
