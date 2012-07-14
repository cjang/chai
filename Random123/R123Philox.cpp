// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0
//
// This code is derived from the R123Philox library version 1.06 .
//
// The design of the R123Philox library relies on compile-time metaprogramming
// using the C preprocessor and awk. This is fine for static compilation. It is
// impractical for a JIT that dynamically generates and compiles OpenCL
// kernels.
//
// OpenCL from the R123Philox library version 1.06 is generated programmatically
// by this C++ class.
//
// The ".i" files are copied from the generated pi_opencl_kernel.i and
// time_opencl_kernel.i files in R123Philox-1.06/examples/ . These files are
// unused but included for reference and to retain the original copyright of
// D. E. Shaw Research on the R123Philox library.

#include <map>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#include "PrecType.hpp"
#include "R123Philox.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// R123Philox OpenCL Philox PRNG

void R123Philox::_mulhilo(ostream& os,
                          const size_t PR) const
{
    os << "inline " << uit(PR) << " mulhilo" << PR
       << "("
       << uit(PR) << " a, "
       << uit(PR) << " b, "
       << uit(PR) << "* hip)" << endl
       << "{" << endl;

    switch (PR)
    {
        case (32) :
            os << "  " << uit(64) << " product = (("
               << uit(64) << ")a) * (("
               << uit(64) << ")b);" << endl
               << "  *hip = product>>32;" << endl
               << "  return (" << uit(32) << ")product;" << endl;
            break;

        case (64) :
            os << "  *hip = mul_hi(a, b);" << endl
               << "  return a*b;" << endl;
            break;
    }

    os << "}" << endl;
}

void R123Philox::_bumpkey(ostream& os,
                          const size_t PR,
                          const size_t VL) const
{
    const size_t vecLenHalf = VL >> 1;

    os << "inline " << _SR123 << vxp(vecLenHalf, PR)
       << " _philox" << vxp(VL, PR) << "bumpkey("
       << _SR123 << vxp(vecLenHalf, PR) << " key)" << endl
       << "{" << endl;

    vector< string > inckeys;

    switch (PR)
    {
        case (32) :
            switch (VL)
            {
                case (2) :
                    inckeys.push_back("0x9E3779B9");
                    break;

                case (4) :
                    inckeys.push_back("0x9E3779B9");
                    inckeys.push_back("0xBB67AE85");
                    break;
            }
            break;

        case (64) :
            switch (VL)
            {
                case (2) :
                    inckeys.push_back("0x9E3779B97F4A7C15UL");
                    break;

                case (4) :
                    inckeys.push_back("0x9E3779B97F4A7C15UL");
                    inckeys.push_back("0xBB67AE8584CAA73BUL");
                    break;
            }
            break;
    }

    for (size_t i = 0; i < inckeys.size(); i++)
    {
        os << "  key.v[" << i << "] += ((" << uit(PR) << ")("
           << inckeys[i] << "));" << endl;
    }

    os << "  return key;" << endl
       << "}" << endl;
}

void R123Philox::_roundFun(ostream& os,
                           const size_t PR,
                           const size_t VL) const
{
    const size_t vecLenHalf = VL >> 1;

    stringstream ss;

    ss << "inline " << _SR123 << vxp(VL, PR)
       << " _philox" << vxp(VL, PR) << "round("
       << _SR123 << vxp(VL, PR) << " ctr, "
       << _SR123 << vxp(vecLenHalf, PR) << " key)";

    const string funhead = ss.str();

    os << funhead << " __attribute__((always_inline));" << endl
       << funhead << endl
       << "{" << endl;

    switch (VL)
    {
        case (2) :
            os << "  " << uit(PR) << " hi;" << endl;
            break;

        case (4) :
            os << "  " << uit(PR) << " hi0;" << endl
               << "  " << uit(PR) << " hi1;" << endl;
            break;
    }

    vector< string > hiloNums;

    switch (PR)
    {
        case (32) :
            switch (VL)
            {
                case (2) :
                    hiloNums.push_back("0xd256d193");
                    break;

                case (4) :
                    hiloNums.push_back("0xD2511F53");
                    hiloNums.push_back("0xCD9E8D57");
                    break;
            }
            break;

        case (64) :
            switch (VL)
            {
                case (2) :
                    hiloNums.push_back("0xD2B74407B1CE6E93UL");
                    break;

                case (4) :
                    hiloNums.push_back("0xD2E7470EE14C6C93UL");
                    hiloNums.push_back("0xCA5A826395121157UL");
                    break;
            }
            break;
    }

    const bool loNum = 1 < hiloNums.size();

    for (size_t i = 0; i < hiloNums.size(); i++)
    {
        os << "  " << uit(PR) << " lo";

        if (loNum)
            os << i;

        os << " = mulhilo" << PR
           << "(((" << uit(PR) << ")" << hiloNums[i]
           << "), ctr.v[" << (i<<1) << "], &hi";

        if (loNum)
            os << i;

        os << ");" << endl;
    }

    os << "  " << _SR123 << vxp(VL, PR) << " out = { {";

    if (loNum)
    {
        os << "hi1 ^ ctr.v[1] ^ key.v[0], "
              "lo1, "
              "hi0 ^ ctr.v[3] ^ key.v[1], lo0";
    }
    else
    {
        os << "hi ^ key.v[0] ^ ctr.v[1], lo";
    }

    os << "} };" << endl
       << "  return out;" << endl
       << "}" << endl;
}

void R123Philox::_roundEnum(ostream& os,
                            const size_t PR,
                            const size_t VL) const
{
    os << "enum r123_enum_philox" << vxp(VL, PR)
       << " { philox" << vxp(VL, PR) << "_rounds = 10 };" << endl;
}

void R123Philox::_typedef(ostream& os,
                          const size_t PR,
                          const size_t VL) const
{
    const size_t vecLenHalf = VL >> 1;

    os << "typedef " << _SR123 << vxp(VL, PR)
       << " philox" << vxp(VL, PR) << "_ctr_t;" << endl
       << "typedef " << _SR123 << vxp(vecLenHalf, PR)
       << " philox" << vxp(VL, PR) << "_key_t;" << endl
       << "typedef " << _SR123 << vxp(vecLenHalf, PR)
       << " philox" << vxp(VL, PR) << "_ukey_t;" << endl;
}

void R123Philox::_keyinit(ostream& os,
                          const size_t PR,
                          const size_t VL) const
{
    os << "inline philox" << vxp(VL, PR) << "_key_t "
          "philox" << vxp(VL, PR) << "keyinit("
          "philox" << vxp(VL, PR) << "_ukey_t uk) "
          "{ return uk; }" << endl;
}

void R123Philox::_R(ostream& os,
                    const size_t PR,
                    const size_t VL) const
{
    stringstream ss;

    ss << "philox" << vxp(VL, PR);

    const string philoxName = ss.str();

    ss.str(string());

    ss << "inline "
       << philoxName << "_ctr_t "
       << philoxName << "_R(unsigned int R, "
       << philoxName << "_ctr_t ctr, "
       << philoxName << "_key_t key)";

    const string funhead = ss.str();

    os << funhead << " __attribute__((always_inline));" << endl
       << funhead << endl
       << "{" << endl
       << "  if (R > 0) { ctr = _" << philoxName << "round(ctr, key); }"
       << endl;

    for (size_t i = 1; i < 16; i++)
    {
        os << "  if (R > " << i << ") {"
              " key = _" << philoxName << "bumpkey(key);"
              " ctr = _" << philoxName << "round(ctr, key); }"
           << endl;
    }

    os << "  return ctr;" << endl
       << "}" << endl;
}

void R123Philox::_entry(ostream& os,
                        const size_t PR,
                        const size_t VL) const
{
    stringstream ss;

    ss << "philox" << vxp(VL, PR);

    const string philoxName = ss.str();

    ss.str(string());

    ss << "inline " << philoxName << "_ctr_t " << philoxName
       << "(" << philoxName << "_ctr_t in, " << philoxName << "_key_t k)";

    const string funhead = ss.str();

    os << funhead << " __attribute((always_inline));" << endl
       << funhead << " { return "
       << philoxName << "_R(" << philoxName << "_rounds, in, k); }" << endl;

    map< size_t, string > wrapTypes; // keys come from PrecType sizeCode
    switch (PR)
    {
        case (32) : // need cast to force compile-time symbol resolution
            wrapTypes[ static_cast<size_t>(PrecType::UInt32) ] = "uint";
            wrapTypes[ static_cast<size_t>(PrecType::Int32) ] = "int";
            wrapTypes[ static_cast<size_t>(PrecType::Float) ] = "float";
            break;

        case (64) : // need cast to force compile-time symbol resolution
            wrapTypes[ static_cast<size_t>(PrecType::Double) ] = "double";
            break;
    }

    map< size_t, string > wrapVectorLengths;
    switch (VL)
    {
        case (2) :
            wrapVectorLengths[1] = "";
            wrapVectorLengths[2] = "2";
            break;

        case (4) :
            wrapVectorLengths[4] = "4";
            break;
    }

    const string baseUIntType = (64 == PR ? uit(64) : uit(32));
    const string unionUIntType = (64 == PR ? "ulong" : "uint");
    const string unionIntType = (64 == PR ? "long" : "int");
    const string unionFloatType = (64 == PR ? "double" : "float");

    for (map< size_t, string >::const_iterator
         precIT = wrapTypes.begin(); precIT != wrapTypes.end(); precIT++)
    {
        const size_t precTypeSizeCode = (*precIT).first;
        const string precTypePrimitiveName = (*precIT).second;

        for (map< size_t, string >::const_iterator
             vlenIT = wrapVectorLengths.begin();
             vlenIT != wrapVectorLengths.end();
             vlenIT++)
        {
            const size_t vecLength = (*vlenIT).first;
            const string vecLengthSuffix = (*vlenIT).second;

            // uniform distribution

            ss.str(string());

            ss << "inline "
               << precTypePrimitiveName << vecLengthSuffix
               << " philox_uniform_"
               << precTypeSizeCode << "_" << vecLength << "("
               << baseUIntType << " tid, "
               << baseUIntType << " idx)";

            os << ss.str() << " __attribute__((always_inline));" << endl
               << ss.str() << " {" << endl
               << "  " << philoxName << "_key_t rngKey = ";

            if (64 == PR)
                os << "{{ tid }};";
            else
                os << "{{ tid, 0xdecafbad }};";

            os << endl
               << "  " << philoxName << "_ctr_t rngCtr = ";

            if (64 == PR)
                os << "{{ idx, 0xf00dcafe }};";
            else
                os << "{{ idx, 0xf00dcafe, 0xdeadbeef, 0xbeeff00d }};";

            os << endl
               << "union { "
               << philoxName << "_ctr_t c; "
               << unionUIntType << VL << " ui; "
               << unionIntType << VL << " ii; } u;" << endl
               << "u.c = " << philoxName << "(rngCtr, rngKey);" << endl
               << "return ";

            switch (precTypeSizeCode)
            {
                case (0) : // uint
                    os << "u.ui";
                    if (1 == vecLength) os << ".s0";
                    break;

                case (1) : // int
                    os << "u.ii";
                    if (1 == vecLength) os << ".s0";
                    break;

                case (4) : // float
                case (8) : // double
                    os << "convert_"
                       << precTypePrimitiveName << vecLengthSuffix
                       << "(u.ii";
                    if (1 == vecLength) os << ".s0";

                    // Based on idea from:
                    //
                    //   Conversion of High-Period Random Numbers
                    //   to Floating Point
                    //   JURGEN A DOORNIK
                    //   University of Oxford
                    //
                    // and the high precision calculator at:
                    //
                    //   http://keisan.casio.com/has10/Free.cgi

                    os << ") * ("
                       << precTypePrimitiveName << vecLengthSuffix
                       << ")("
                       << (64 == PR ? _M_RAN_INVM64 : _M_RAN_INVM32)
                       << ") + ("
                       << precTypePrimitiveName << vecLengthSuffix
                       << ")(0.5)";
                    break;
            }

            os << ";" << endl
               << "}" << endl;

            // normal distribution (only makes sense for floating point)

            if (4 == precTypeSizeCode || 8 == precTypeSizeCode)
            {
                ss.str(string());

                ss << "inline "
                   << precTypePrimitiveName << vecLengthSuffix
                   << " philox_normal_"
                   << precTypeSizeCode << "_" << vecLength << "("
                   << baseUIntType << " tid, "
                   << baseUIntType << " idx)";

                os << ss.str() << " __attribute__((always_inline));" << endl
                   << ss.str() << " {" << endl
                   << "  " << philoxName << "_key_t rngKey = ";

                if (64 == PR)
                    os << "{{ tid }};";
                else
                    os << "{{ tid, 0xdecafbad }};";

                os << endl
                   << "  " << philoxName << "_ctr_t rngCtr = ";

                if (64 == PR)
                    os << "{{ idx, 0xf00dcafe }};";
                else
                    os << "{{ idx, 0xf00dcafe, 0xdeadbeef, 0xbeeff00d }};";

                os << endl
                   << "union { "
                   << philoxName << "_ctr_t c; "
                   << unionIntType << VL << " ii; "
                   << unionFloatType << VL << " fp; } u;" << endl
                   << "u.c = " << philoxName << "(rngCtr, rngKey);" << endl
                   << "u.fp = convert_" << unionFloatType << VL << "(u.ii)"
                      " * (" << unionFloatType << VL << ")("
                   << (64 == PR ? _M_RAN_INVM64 : _M_RAN_INVM32)
                   << ") + ("
                   << unionFloatType << VL << ")(0.5);" << endl
                   << "return ";

                switch (vecLength)
                {
                    case (1) :
                        os << "sqrt(-2 * log(u.fp.s0)) * sin(2 * "
                           << M_PI
                           << " * u.fp.s1)";
                        break;

                    case (2) :
                        os << "(" << precTypePrimitiveName << "2)("
                              "sqrt(-2 * log(u.fp.s0)) * sin(2 * "
                           << M_PI
                           << " * u.fp.s1)"
                              ", "
                              "sqrt(-2 * log(u.fp.s0)) * cos(2 * "
                           << M_PI
                           << " * u.fp.s1)"
                              ")";
                        break;

                    case (4) :
                        os << "(" << precTypePrimitiveName << "4)("
                              "sqrt(-2 * log(u.fp.s0)) * sin(2 * "
                           << M_PI
                           << " * u.fp.s1)"
                              ", "
                              "sqrt(-2 * log(u.fp.s0)) * cos(2 * "
                           << M_PI
                           << " * u.fp.s1)"
                              ", "
                              "sqrt(-2 * log(u.fp.s2)) * sin(2 * "
                           << M_PI
                           << " * u.fp.s3)"
                              ", "
                              "sqrt(-2 * log(u.fp.s2)) * cos(2 * "
                           << M_PI
                           << " * u.fp.s3)"
                              ")";
                        break;
                }

                os << ";" << endl
                   << "}" << endl;
            }
        }
    }
}

R123Philox::R123Philox(const set< size_t >& precSet,
                       const set< size_t >& vecLenSet)
    : R123Util(),
      _precSet(precSet),
      _vecLenSet(vecLenSet) { }

void R123Philox::print(ostream& os) const
{
    for (set< size_t >::const_iterator
         PR = _precSet.begin(); PR != _precSet.end(); PR++)
    {
        _mulhilo(os << endl, *PR);

        for (set< size_t >::const_iterator
             VL = _vecLenSet.begin(); VL != _vecLenSet.end(); VL++)
        {
            _bumpkey(os << endl, *PR, *VL);
            _roundFun(os << endl, *PR, *VL);
            _roundEnum(os << endl, *PR, *VL);
            _typedef(os << endl, *PR, *VL);
            _keyinit(os << endl, *PR, *VL);
            _R(os << endl, *PR, *VL);
            _entry(os << endl, *PR, *VL);
        }
    }
}

}; // namespace chai_internal
