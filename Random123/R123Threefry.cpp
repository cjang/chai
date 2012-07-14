// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0
//
// This code is derived from the R123Threefry library version 1.06 .
//
// The design of the R123Threefry library relies on compile-time metaprogramming
// using the C preprocessor and awk. This is fine for static compilation. It is
// impractical for a JIT that dynamically generates and compiles OpenCL
// kernels.
//
// OpenCL from the R123Threefry library version 1.06 is generated programmatically
// by this C++ class.
//
// The ".i" files are copied from the generated pi_opencl_kernel.i and
// time_opencl_kernel.i files in R123Threefry-1.06/examples/ . These files are
// unused but included for reference and to retain the original copyright of
// D. E. Shaw Research on the R123Threefry library.

#include <map>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#include "PrecType.hpp"
#include "R123Threefry.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// R123Threefry OpenCL Threefry PRNG

void R123Threefry::_enum(ostream& os,
                         const size_t PR,
                         const size_t VL) const
{
    os << "enum r123_enum_threefry" << pxv(PR, VL) << endl
       << "{" << endl;

    vector< string > enumKeys;
    vector< size_t > enumVals;

    switch (PR)
    {
        case (32) :
            switch (VL)
            {
                case (2) :
                    enumKeys.push_back("0_0"); enumVals.push_back(13);
                    enumKeys.push_back("1_0"); enumVals.push_back(15);
                    enumKeys.push_back("2_0"); enumVals.push_back(26);
                    enumKeys.push_back("3_0"); enumVals.push_back(6);
                    enumKeys.push_back("4_0"); enumVals.push_back(17);
                    enumKeys.push_back("5_0"); enumVals.push_back(29);
                    enumKeys.push_back("6_0"); enumVals.push_back(16);
                    enumKeys.push_back("7_0"); enumVals.push_back(24);
                    break;

                case (4) :
                    enumKeys.push_back("0_0"); enumVals.push_back(10);
                    enumKeys.push_back("1_0"); enumVals.push_back(11);
                    enumKeys.push_back("2_0"); enumVals.push_back(13);
                    enumKeys.push_back("3_0"); enumVals.push_back(23);
                    enumKeys.push_back("4_0"); enumVals.push_back(6);
                    enumKeys.push_back("5_0"); enumVals.push_back(17);
                    enumKeys.push_back("6_0"); enumVals.push_back(25);
                    enumKeys.push_back("7_0"); enumVals.push_back(18);
                    enumKeys.push_back("0_1"); enumVals.push_back(26);
                    enumKeys.push_back("1_1"); enumVals.push_back(21);
                    enumKeys.push_back("2_1"); enumVals.push_back(27);
                    enumKeys.push_back("3_1"); enumVals.push_back(5);
                    enumKeys.push_back("4_1"); enumVals.push_back(20);
                    enumKeys.push_back("5_1"); enumVals.push_back(11);
                    enumKeys.push_back("6_1"); enumVals.push_back(10);
                    enumKeys.push_back("7_1"); enumVals.push_back(20);
                    break;
            }
            break;

        case (64) :
            switch (VL)
            {
                case (2) :
                    enumKeys.push_back("0_0"); enumVals.push_back(16);
                    enumKeys.push_back("1_0"); enumVals.push_back(42);
                    enumKeys.push_back("2_0"); enumVals.push_back(12);
                    enumKeys.push_back("3_0"); enumVals.push_back(31);
                    enumKeys.push_back("4_0"); enumVals.push_back(16);
                    enumKeys.push_back("5_0"); enumVals.push_back(32);
                    enumKeys.push_back("6_0"); enumVals.push_back(24);
                    enumKeys.push_back("7_0"); enumVals.push_back(21);
                    break;

                case (4) :
                    enumKeys.push_back("0_0"); enumVals.push_back(14);
                    enumKeys.push_back("1_0"); enumVals.push_back(52);
                    enumKeys.push_back("2_0"); enumVals.push_back(23);
                    enumKeys.push_back("3_0"); enumVals.push_back(5);
                    enumKeys.push_back("4_0"); enumVals.push_back(25);
                    enumKeys.push_back("5_0"); enumVals.push_back(46);
                    enumKeys.push_back("6_0"); enumVals.push_back(58);
                    enumKeys.push_back("7_0"); enumVals.push_back(32);
                    enumKeys.push_back("0_1"); enumVals.push_back(16);
                    enumKeys.push_back("1_1"); enumVals.push_back(57);
                    enumKeys.push_back("2_1"); enumVals.push_back(40);
                    enumKeys.push_back("3_1"); enumVals.push_back(37);
                    enumKeys.push_back("4_1"); enumVals.push_back(33);
                    enumKeys.push_back("5_1"); enumVals.push_back(12);
                    enumKeys.push_back("6_1"); enumVals.push_back(22);
                    enumKeys.push_back("7_1"); enumVals.push_back(32);
                    break;
            }
            break;
    }

    for (size_t i = 0; i < enumKeys.size(); i++)
    {
        os << "  R_" << pxv(PR, VL) << "_" << enumKeys[i]
           << "=" << enumVals[i];

        if (i < enumKeys.size()-1)
            os << ",";

        os << endl;
    }

    os << "};" << endl;
}

void R123Threefry::_rotL(ostream& os,
                         const size_t PR) const
{
    stringstream ss;

    ss << "inline " << uit(PR) << " RotL_" << PR
       << "(" << uit(PR) << " x, unsigned int N)";

    const string funhead = ss.str();

    os << funhead << " __attribute__((always_inline));" << endl
       << funhead
       << " { return (x << (N & " << (PR-1) << ")) | "
                    "(x >> ((" << PR << "-N) & " << (PR-1) << ")); }" << endl;
}

void R123Threefry::_typedef(ostream& os,
                            const size_t PR,
                            const size_t VL) const
{
    stringstream ss;

    ss << "typedef " << _SR123 << vxp(VL, PR)
       << " threefry" << vxp(VL, PR);

    os << ss.str() << "_ctr_t;" << endl
       << ss.str() << "_key_t;" << endl
       << ss.str() << "_ukey_t;" << endl;
}

void R123Threefry::_keyinit(ostream& os,
                            const size_t PR,
                            const size_t VL) const
{
    stringstream ss;

    ss << "threefry" << vxp(VL, PR);

    os << "inline "
       << ss.str() << "_key_t "
       << ss.str() << "keyinit("
       << ss.str() << "_ukey_t uk) { return uk; }" << endl;
}

void R123Threefry::_R(ostream& os,
                      const size_t PR,
                      const size_t VL) const
{
    stringstream ss;

    ss << "threefry" << vxp(VL, PR);

    const string threefryName = ss.str();

    ss.str(string());

    ss << "inline "
       << threefryName << "_ctr_t "
       << threefryName << "_R(unsigned int Nrounds, "
       << threefryName << "_ctr_t in, "
       << threefryName << "_key_t k)";

    const string funhead = ss.str();

    os << funhead << " __attribute((always_inline));" << endl
       << funhead << endl
       << "{" << endl;

    os << "  " << threefryName << "_ctr_t X;" << endl
       << "  " << uit(PR) << " ks[" << VL << "+1];" << endl
       << "  int i;" << endl
       << "  ks[" << VL << "] = ";

    switch (PR)
    {
        case (32) :
            os << "0x1BD11BDA";
            break;

        case (64) :
            os << "((0xA9FC1A22) + (((" << uit(64) << ")(0x1BD11BDA)) << 32))";
            break;
    }

    os << ";" << endl
       << "  for (i = 0; i < " << VL << "; i++)" << endl
       << "  {" << endl
       << "    ks[i] = k.v[i];" << endl
       << "    X.v[i] = in.v[i];" << endl
       << "    ks[" << VL << "] ^= k.v[i];" << endl
       << "  }" << endl;

    for (size_t i = 0; i < VL; i++)
    {
        os << "  X.v[" << i << "] += ks[" << i << "];" << endl;
    }

    const size_t Nrounds = (2 == VL) ? 32 : 72;
    const size_t resetRotL = (2 == VL) ? 5 : -1;
    size_t majorRotL = 0;
    size_t countQuad = 0;

    for (size_t i = 0; i < Nrounds; i+=4)
    {
        if (resetRotL == countQuad++)
            majorRotL = 0;

        for (size_t j = i; j < i+4; j++)
        {
            os << "  if (Nrounds > " << i << ") {";

            size_t minorRotL = 0;

            for (size_t k = 0; k < VL; k+=2)
            {
                os << " X.v[" << k << "] += X.v[" << (k+1) << "];"
                      " X.v[" << (k+1) << "] ="
                      " RotL_" << PR << "(X.v[" << (k+1) << "],"
                      " R_" << pxv(PR, VL)
                   << "_" << majorRotL << "_" << minorRotL << ");"
                      " X.v[" << (k+1) << "] ^= X.v[" << k << "];";

                minorRotL++;
            }

            majorRotL++;

            os << " }"
               << endl;
        }

        if (8 == majorRotL)
            majorRotL = 0;

        os << "  if (Nrounds > " << (i+3) << ") {";

        for (size_t j = 0; j < VL; j++)
        {
            os << " X.v[" << j << "] +="
                  " ks[" << ((countQuad+j) % (VL+1)) << "];";
        }

        os << " X.v[" << (VL-1) << "] += " << countQuad << "; }"
           << endl;
    }

    os << "  return X;" << endl
       << "}" << endl;
}

void R123Threefry::_roundEnum(ostream& os,
                              const size_t PR,
                              const size_t VL) const
{
    os << "enum r123_enum_threefry" << vxp(VL, PR)
       << " { threefry" << vxp(VL, PR) << "_rounds = 20 };" << endl;
}

void R123Threefry::_entry(ostream& os,
                          const size_t PR,
                          const size_t VL) const
{
    stringstream ss;

    ss << "threefry" << vxp(VL, PR);

    const string threefryName = ss.str();

    ss.str(string());

    ss << "inline "
       << threefryName << "_ctr_t "
       << threefryName << "("
       << threefryName << "_ctr_t in, "
       << threefryName << "_key_t k)";

    const string funhead = ss.str();

    os << funhead << " __attribute((always_inline));" << endl
       << funhead << " { return " << threefryName
       << "_R(" << threefryName << "_rounds, in, k); }" << endl;

    map< size_t, string > wrapTypes; // keys come from PrecType sizeCode
    switch (PR)
    {
        case (32) :
            wrapTypes[ static_cast<size_t>(PrecType::UInt32) ] = "uint";
            wrapTypes[ static_cast<size_t>(PrecType::Int32) ] = "int";
            wrapTypes[ static_cast<size_t>(PrecType::Float) ] = "float";
            break;

        case (64) :
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
         precIT = wrapTypes.begin();
         precIT != wrapTypes.end();
         precIT++)
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
               << " threefry_uniform_"
               << precTypeSizeCode << "_" << vecLength << "("
               << baseUIntType << " tid, "
               << baseUIntType << " idx)";

            os << ss.str() << " __attribute__((always_inline));" << endl
               << ss.str() << " {" << endl
               << "  " << threefryName << "_key_t rngKey = ";

            if (64 == PR)
                os << "{{ tid, 0xdecafbad }};";
            else
                os << "{{ tid, 0xdecafbad, 0xfacebead, 0x12345678 }};";

            os << endl
               << "  " << threefryName << "_ctr_t rngCtr = ";

            if (64 == PR)
                os << "{{ idx, 0xf00dcafe }};";
            else
                os << "{{ idx, 0xf00dcafe, 0xdeadbeef, 0xbeeff00d }};";

            os << endl
               << "union { "
               << threefryName << "_ctr_t c; "
               << unionUIntType << VL << " ui; "
               << unionIntType << VL << " ii; } u;" << endl
               << "u.c = " << threefryName << "(rngCtr, rngKey);" << endl
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
                   << " threefry_normal_"
                   << precTypeSizeCode << "_" << vecLength << "("
                   << baseUIntType << " tid, "
                   << baseUIntType << " idx)";

                os << ss.str() << " __attribute__((always_inline));" << endl
                   << ss.str() << " {" << endl
                   << "  " << threefryName << "_key_t rngKey = ";

                if (64 == PR)
                    os << "{{ tid, 0xdecafbad }};";
                else
                    os << "{{ tid, 0xdecafbad, 0xfacebead, 0x12345678 }};";

                os << endl
                   << "  " << threefryName << "_ctr_t rngCtr = ";

                if (64 == PR)
                    os << "{{ idx, 0xf00dcafe }};";
                else
                    os << "{{ idx, 0xf00dcafe, 0xdeadbeef, 0xbeeff00d }};";

                os << endl
                   << "union { "
                   << threefryName << "_ctr_t c; "
                   << unionIntType << VL << " ii; "
                   << unionFloatType << VL << " fp; } u;" << endl
                   << "u.c = " << threefryName << "(rngCtr, rngKey);" << endl
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

R123Threefry::R123Threefry(const set< size_t >& precSet,
                           const set< size_t >& vecLenSet)
    : R123Util(),
      _precSet(precSet),
      _vecLenSet(vecLenSet) { }

void R123Threefry::print(ostream& os) const
{
    for (set< size_t >::const_iterator
         PR = _precSet.begin(); PR != _precSet.end(); PR++)
    {
        _rotL(os << endl, *PR);

        for (set< size_t >::const_iterator
             VL = _vecLenSet.begin(); VL != _vecLenSet.end(); VL++)
        {
            _enum(os << endl, *PR, *VL);
            _typedef(os << endl, *PR, *VL);
            _keyinit(os << endl, *PR, *VL);
            _R(os << endl, *PR, *VL);
            _roundEnum(os << endl, *PR, *VL);
            _entry(os << endl, *PR, *VL);
        }
    }
}

}; // namespace chai_internal
