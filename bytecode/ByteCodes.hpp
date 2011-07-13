// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_BYTE_CODES_HPP_
#define _CHAI_BYTE_CODES_HPP_

#include <stdint.h>

namespace chai_internal {

////////////////////////////////////////
// operation bytecodes

struct ByteCodes
{
    static const uint32_t abs                     =  0;
    static const uint32_t cond                    =  1;
    static const uint32_t convert_f32             =  2;
    static const uint32_t convert_f64             =  3;
    static const uint32_t dot_product             =  4;

    static const uint32_t exp                     =  5;
    static const uint32_t gather1_floor           =  6;
    static const uint32_t gather2_floor           =  7;
    static const uint32_t index1_f32              =  8;
    static const uint32_t index1_f64              =  9;

    static const uint32_t index2_f32              = 10;
    static const uint32_t index2_f64              = 11;
    static const uint32_t make1_f32               = 12;
    static const uint32_t make1_f64               = 13;
    static const uint32_t make2_f32               = 14;

    static const uint32_t make2_f64               = 15;
    static const uint32_t matmul                  = 16;
    static const uint32_t max                     = 17;
    static const uint32_t mean                    = 18;
    static const uint32_t min                     = 19;

    static const uint32_t negate                  = 20;
    static const uint32_t ones1_f32               = 21;
    static const uint32_t ones1_f64               = 22;
    static const uint32_t ones2_f32               = 23;
    static const uint32_t ones2_f64               = 24;

    static const uint32_t operatorADD             = 25;
    static const uint32_t operatorAND             = 26;
    static const uint32_t operatorDIV             = 27;
    static const uint32_t operatorEQ              = 28;
    static const uint32_t operatorGE              = 29;

    static const uint32_t operatorGT              = 30;
    static const uint32_t operatorLE              = 31;
    static const uint32_t operatorLT              = 32;
    static const uint32_t operatorMUL             = 33;
    static const uint32_t operatorNE              = 34;

    static const uint32_t operatorOR              = 35;
    static const uint32_t operatorSUB             = 36;
    static const uint32_t read_scalar_f32         = 37;
    static const uint32_t read_scalar_f64         = 38;
    static const uint32_t read1_f32               = 39;

    static const uint32_t read1_f64               = 40;
    static const uint32_t read2_f32               = 41;
    static const uint32_t read2_f64               = 42;
    static const uint32_t rng_normal_make_f32     = 43;
    static const uint32_t rng_normal_make_f64     = 44;

    static const uint32_t rng_uniform_make_f32    = 45;
    static const uint32_t rng_uniform_make_f64    = 46;
    static const uint32_t scalar_f32              = 47;
    static const uint32_t scalar_f64              = 48;
    static const uint32_t sqrt                    = 49;

    static const uint32_t sum                     = 50;
    static const uint32_t zeros1_f32              = 51;
    static const uint32_t zeros1_f64              = 52;
    static const uint32_t zeros2_f32              = 53;
    static const uint32_t zeros2_f64              = 54;

    static const uint32_t NUMBER_CODES = 55;

    static const char* name(const uint32_t);
};

}; // namespace chai_internal

#endif
