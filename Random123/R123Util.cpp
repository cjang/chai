// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0
//
// This code is derived from the R123Util library version 1.06 .
//
// The design of the R123Util library relies on compile-time metaprogramming
// using the C preprocessor and awk. This is fine for static compilation. It is
// impractical for a JIT that dynamically generates and compiles OpenCL
// kernels.
//
// OpenCL from the R123Util library version 1.06 is generated programmatically
// by this C++ class.
//
// The ".i" files are copied from the generated pi_opencl_kernel.i and
// time_opencl_kernel.i files in R123Util-1.06/examples/ . These files are
// unused but included for reference and to retain the original copyright of
// D. E. Shaw Research on the R123Util library.

#include "R123Util.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// R123Util OpenCL utility functions

// uint ## PR ## _t
const char* R123Util::uit(const size_t PR) const
{
    switch (PR)
    {
        case (8) : return "uint8_t";
        case (16) : return "uint16_t";
        case (32) : return "uint32_t";
        case (64) : return "uint64_t";
    }
}

// VL ## x ## PR
const char* R123Util::vxp(const size_t VL,
                          const size_t PR) const
{
    switch (VL)
    {
        case (1) :
            switch (PR)
            {
                case (32) : return "1x32";
                case (64) : return "1x64";
            }

        case (2) :
            switch (PR)
            {
                case (32) : return "2x32";
                case (64) : return "2x64";
            }

        case (4) :
            switch (PR)
            {
                case (32) : return "4x32";
                case (64) : return "4x64";
            }
    }
}

// PR ## x ## VL
const char* R123Util::pxv(const size_t PR,
                          const size_t VL) const
{
    switch (VL)
    {
        case (1) :
            switch (PR)
            {
                case (32) : return "32x1";
                case (64) : return "64x1";
            }

        case (2) :
            switch (PR)
            {
                case (32) : return "32x2";
                case (64) : return "64x2";
            }

        case (4) :
            switch (PR)
            {
                case (32) : return "32x4";
                case (64) : return "64x4";
            }
    }
}

R123Util::R123Util(void)
    : _M_RAN_INVM32("2.328306436538696289063E-10"),
      _M_RAN_INVM64("5.421010862427522170037E-20"),
      _SR123("struct r123array") { }

R123Util::~R123Util(void) { }

}; // namespace chai_internal
