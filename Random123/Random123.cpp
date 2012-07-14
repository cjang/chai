// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0
//
// This code is derived from the Random123 library version 1.06 .
//
// The design of the Random123 library relies on compile-time metaprogramming
// using the C preprocessor and awk. This is fine for static compilation. It is
// impractical for a JIT that dynamically generates and compiles OpenCL
// kernels.
//
// OpenCL from the Random123 library version 1.06 is generated programmatically
// by this C++ class.
//
// The ".i" files are copied from the generated pi_opencl_kernel.i and
// time_opencl_kernel.i files in Random123-1.06/examples/ . These files are
// unused but included for reference and to retain the original copyright of
// D. E. Shaw Research on the Random123 library.

#include "R123Philox.hpp"
#include "R123Threefry.hpp"
#include "Random123.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// Random123 OpenCL for the JIT

// integer typedefs
void Random123::typedefDecl(ostream& os) const
{
    os << "typedef ulong " << uit(64) << ";" << endl
       << "typedef uint " << uit(32) << ";" << endl
       << "typedef uchar " << uit(8) << ";" << endl;
}

// array
void Random123::arrayDecl(ostream& os) const
{
    set< size_t > vecLens;

    if (_philox)
    {
        for (set< size_t >::const_iterator
             it = _vecLenSet.begin(); it != _vecLenSet.end(); it++)
        {
            vecLens.insert( (*it) >> 1 );
        }
    }

    vecLens.insert(_vecLenSet.begin(), _vecLenSet.end());

    for (set< size_t >::const_iterator
         PR = _precSet.begin(); PR != _precSet.end(); PR++)
    {
        for (set< size_t >::const_iterator
             VL = vecLens.begin(); VL != vecLens.end(); VL++)
        {
            arrayDecl(os << endl, *PR, *VL);
        }
    }
}

// array
void Random123::arrayDecl(ostream& os,
                          const size_t PR,
                          const size_t VL) const
{
    os << _SR123 << vxp(VL, PR) << " { " << uit(PR) << " v[" << VL << "]; };";
}

Random123::Random123(void)
    : R123Util(),
      _precSet(),
      _vecLenSet(),
      _philox(false),
      _threefry(false) { }

void Random123::prec32(void)
{
    _precSet.insert(32);
}

void Random123::prec64(void)
{
    _precSet.insert(64);
}

void Random123::vecLength2(void)
{
    _vecLenSet.insert(2);
}

void Random123::vecLength4(void)
{
    _vecLenSet.insert(4);
}

void Random123::philox(void)
{
    _philox = true;
}

void Random123::threefry(void)
{
    _threefry = true;
}

void Random123::print(ostream& os)
{
    if (!_philox && !_threefry)
        return;

    // integer typedefs
    typedefDecl(os);

    // array declaration
    arrayDecl(os);

    // philox RNG
    if (_philox)
    {
        R123Philox gen(_precSet, _vecLenSet);

        gen.print(os << endl);
    }

    // threefry RNG
    if (_threefry)
    {
        R123Threefry gen(_precSet, _vecLenSet);

        gen.print(os << endl);
    }
}

}; // namespace chai_internal
