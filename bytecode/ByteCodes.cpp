// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"

namespace chai_internal {

static const char* CODE_NAME[] =
{
    // 0
    "abs",
    "cond",
    "convert_f32",
    "convert_f64",
    "dot_product",

    // 5
    "exp",
    "gather1_floor",
    "gather2_floor",
    "index1_f32",
    "index1_f64",

    // 10
    "index2_f32",
    "index2_f64",
    "make1_f32",
    "make1_f64",
    "make2_f32",

    // 15
    "make2_f64",
    "matmul",
    "max",
    "mean",
    "min",

    // 20
    "negate",
    "ones1_f32",
    "ones1_f64",
    "ones2_f32",
    "ones2_f64",

    // 25
    "operatorADD",
    "operatorAND",
    "operatorDIV",
    "operatorEQ",
    "operatorGE",

    // 30
    "operatorGT",
    "operatorLE",
    "operatorLT",
    "operatorMUL",
    "operatorNE",

    // 35
    "operatorOR",
    "operatorSUB",
    "read_scalar_f32",
    "read_scalar_f64",
    "read1_f32",

    // 40
    "read1_f64",
    "read2_f32",
    "read2_f64",
    "rng_normal_make_f32",
    "rng_normal_make_f64",

    // 45
    "rng_uniform_make_f32",
    "rng_uniform_make_f64",
    "scalar_f32",
    "scalar_f64",
    "sqrt",

    // 50
    "sum",
    "zeros1_f32",
    "zeros1_f64",
    "zeros2_f32",
    "zeros2_f64"
};

const char* ByteCodes::name(const uint32_t i)
{
    return CODE_NAME[i];
}

}; // namespace chai_internal
