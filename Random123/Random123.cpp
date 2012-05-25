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

#include <map>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#include "Random123.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// Random123 OpenCL for the JIT

void Random123::typedefDecl(ostream& os) const
{
    os << "typedef ulong uint64_t;"
       << endl
       << "typedef uint uint32_t;"
       << endl
       << "typedef uchar uint8_t;"
       << endl;
}

void Random123::arrayDecl(ostream& os,
                          const size_t prec,
                          const size_t vlen) const
{
    os << "struct r123array" << vlen << "x" << prec
       << " { uint" << prec << "_t v[" << vlen << "]; };"
       << endl;
}

void Random123::arrayDecl(ostream& os) const
{
    for (set< size_t >::const_iterator
         prec = _precSet.begin();
         prec != _precSet.end();
         prec++)
    {
        for (set< size_t >::const_iterator
             vlen = _vlenSet.begin();
             vlen != _vlenSet.end();
             vlen++)
        {
            arrayDecl(os, *prec, *vlen);

            if (_philox)
            {
                arrayDecl(os, *prec, *vlen >> 1);
            }
        }
    }

    os << endl;
}

void Random123::philoxMulhilo(ostream& os,
                              const size_t prec) const
{
    os << "inline uint" << prec << "_t mulhilo" << prec
       << "(uint" << prec << "_t a,"
          " uint" << prec << "_t b,"
          " uint" << prec << "_t* hip)"
       << endl
       << "{"
       << endl;

    switch (prec)
    {
        case (32) :
            os << "  uint64_t product = ((uint64_t)a) * ((uint64_t)b);"
               << endl
               << "  *hip = product>>32;"
               << endl
               << "  return (uint32_t)product;"
               << endl;
            break;

        case (64) :
            os << "  *hip = mul_hi(a, b);"
               << endl
               << "  return a*b;"
               << endl;
            break;
    }

    os << "}"
       << endl;
}

void Random123::philoxBumpkey(ostream& os,
                              const size_t prec,
                              const size_t vlen) const
{
    const size_t vlenHalf = vlen >> 1;

    os << "inline struct r123array" << vlenHalf << "x" << prec
       << " _philox" << vlen << "x" << prec << "bumpkey("
          "struct r123array" << vlenHalf << "x" << prec << " key)"
       << endl
       << "{"
       << endl;

    vector< string > inckeys;

    switch (prec)
    {
        case (32) :
            switch (vlen)
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
            switch (vlen)
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
        os << "  key.v[" << i << "] += ((uint" << prec << "_t)("
           << inckeys[i] << "));"
           << endl;
    }

    os << "  return key;"
       << endl
       << "}"
       << endl;
}

void Random123::philoxRoundFun(ostream& os,
                               const size_t prec,
                               const size_t vlen) const
{
    const size_t vlenHalf = vlen >> 1;

    stringstream ss;
    ss << "inline struct r123array" << vlen << "x" << prec
       << " _philox" << vlen << "x" << prec << "round("
          "struct r123array" << vlen << "x" << prec << " ctr, "
          "struct r123array" << vlenHalf << "x" << prec << " key)";
    const string funhead = ss.str();

    os << funhead << " __attribute__((always_inline));"
       << endl
       << funhead
       << endl
       << "{"
       << endl;

    switch (vlen)
    {
        case (2) :
            os << "  uint" << prec << "_t hi;"
               << endl;
            break;

        case (4) :
            os << "  uint" << prec << "_t hi0;"
               << endl
               << "  uint" << prec << "_t hi1;"
               << endl;
            break;
    }

    vector< string > hiloNums;

    switch (prec)
    {
        case (32) :
            switch (vlen)
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
            switch (vlen)
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
        os << "  uint" << prec << "_t lo";

        if (loNum)
            os << i;

        os << " = mulhilo" << prec << "(((uint" << prec << "_t)"
           << hiloNums[i]
           << "), ctr.v[" << (i<<1) << "], &hi";

        if (loNum)
            os << i;

        os << ");"
           << endl;
    }

    os << "  struct r123array" << vlen << "x" << prec << " out = { {";

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

    os << "} };"
       << endl
       << "  return out;"
       << endl
       << "}"
       << endl;
}

void Random123::philoxRoundEnum(ostream& os,
                                const size_t prec,
                                const size_t vlen) const
{
    os << "enum r123_enum_philox" << vlen << "x" << prec
       << " { philox" << vlen << "x" << prec << "_rounds = 10 };"
       << endl;
}

void Random123::philoxTypedef(ostream& os,
                              const size_t prec,
                              const size_t vlen) const
{
    const size_t vlenHalf = vlen >> 1;

    os << "typedef struct r123array" << vlen << "x" << prec
       << " philox" << vlen << "x" << prec << "_ctr_t;"
       << endl
       << "typedef struct r123array" << vlenHalf << "x" << prec
       << " philox" << vlen << "x" << prec << "_key_t;"
       << endl
       << "typedef struct r123array" << vlenHalf << "x" << prec
       << " philox" << vlen << "x" << prec << "_ukey_t;"
       << endl;
}

void Random123::philoxKeyinit(ostream& os,
                              const size_t prec,
                              const size_t vlen) const
{
    os << "inline philox" << vlen << "x" << prec << "_key_t "
          "philox" << vlen << "x" << prec << "keyinit("
          "philox" << vlen << "x" << prec << "_ukey_t uk) "
          "{ return uk; }"
       << endl;
}

void Random123::philox_R(ostream& os,
                         const size_t prec,
                         const size_t vlen) const
{
    stringstream ss;

    ss << "philox" << vlen << "x" << prec;
    const string philoxName = ss.str();

    ss.str(string());

    ss << "inline "
       << philoxName << "_ctr_t "
       << philoxName << "_R(unsigned int R, "
       << philoxName << "_ctr_t ctr, "
       << philoxName << "_key_t key)";
    const string funhead = ss.str();

    os << funhead << " __attribute__((always_inline));"
       << endl
       << funhead
       << endl
       << "{"
       << endl
       << "  if (R > 0) {"
          " ctr = _" << philoxName << "round(ctr, key); }"
       << endl;

    for (size_t i = 1; i < 16; i++)
    {
        os << "  if (R > " << i << ") {"
              " key = _" << philoxName << "bumpkey(key);"
              " ctr = _" << philoxName << "round(ctr, key); }"
           << endl;
    }

    os << "  return ctr;"
       << endl
       << "}"
       << endl;
}

void Random123::philoxEntry(ostream& os,
                            const size_t prec,
                            const size_t vlen) const
{
    stringstream ss;

    ss << "philox" << vlen << "x" << prec;
    const string philoxName = ss.str();

    ss.str(string());

    ss << "inline " << philoxName << "_ctr_t "
       << philoxName << "(" << philoxName << "_ctr_t in, "
       << philoxName << "_key_t k)";
    const string funhead = ss.str();

    os << funhead << " __attribute((always_inline));"
       << endl
       << funhead << " { return " << philoxName << "_R(" << philoxName
       << "_rounds, in, k); }"
       << endl;

    map< size_t, string > wrapTypes; // keys come from PrecType sizeCode
    switch (prec)
    {
        case (32) :
            wrapTypes[0] = "uint";
            wrapTypes[1] = "int";
            wrapTypes[4] = "float";
            break;

        case (64) :
            wrapTypes[8] = "double";
            break;
    }

    map< size_t, string > wrapVectorLengths;
    switch (vlen)
    {
        case (2) :
            wrapVectorLengths[1] = "";
            wrapVectorLengths[2] = "2";
            break;

        case (4) :
            wrapVectorLengths[4] = "4";
            break;
    }

    const string baseUIntType = (64 == prec ? "uint64_t" : "uint32_t");
    const string unionUIntType = (64 == prec ? "ulong" : "uint");
    const string unionIntType = (64 == prec ? "long" : "int");
    const string unionFloatType = (64 == prec ? "double" : "float");

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

            os << "inline "
               << precTypePrimitiveName << vecLengthSuffix
               << " philox_uniform_"
               << precTypeSizeCode << "_" << vecLength << "("
               << baseUIntType << " tid, "
               << baseUIntType << " idx) __attribute__((always_inline));"
               << endl
               << "inline "
               << precTypePrimitiveName << vecLengthSuffix
               << " philox_uniform_"
               << precTypeSizeCode << "_" << vecLength << "("
               << baseUIntType << " tid, "
               << baseUIntType << " idx) {"
               << endl
               << "  " << philoxName << "_key_t rngKey = ";

            if (64 == prec)
                os << "{{ tid }};";
            else
                os << "{{ tid, 0xdecafbad }};";

            os << endl
               << "  " << philoxName << "_ctr_t rngCtr = ";

            if (64 == prec)
                os << "{{ idx, 0xf00dcafe }};";
            else
                os << "{{ idx, 0xf00dcafe, 0xdeadbeef, 0xbeeff00d }};";

            os << endl
               << "union { "
               << philoxName << "_ctr_t c; "
               << unionUIntType << vlen << " ui; "
               << unionIntType << vlen << " ii; } u;"
               << endl
               << "u.c = " << philoxName << "(rngCtr, rngKey);"
               << endl
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
                       << (64 == prec ? _M_RAN_INVM64 : _M_RAN_INVM32)
                       << ") + ("
                       << precTypePrimitiveName << vecLengthSuffix
                       << ")(0.5)";
                    break;
            }

            os << ";"
               << endl
               << "}"
               << endl;

            // normal distribution (only makes sense for floating point)

            if (4 == precTypeSizeCode || 8 == precTypeSizeCode)
            {
                os << "inline "
                   << precTypePrimitiveName << vecLengthSuffix
                   << " philox_normal_"
                   << precTypeSizeCode << "_" << vecLength << "("
                   << baseUIntType << " tid, "
                   << baseUIntType << " idx) __attribute__((always_inline));"
                   << endl
                   << "inline "
                   << precTypePrimitiveName << vecLengthSuffix
                   << " philox_normal_"
                   << precTypeSizeCode << "_" << vecLength << "("
                   << baseUIntType << " tid, "
                   << baseUIntType << " idx) {"
                   << endl
                   << "  " << philoxName << "_key_t rngKey = ";

                if (64 == prec)
                    os << "{{ tid }};";
                else
                    os << "{{ tid, 0xdecafbad }};";

                os << endl
                   << "  " << philoxName << "_ctr_t rngCtr = ";

                if (64 == prec)
                    os << "{{ idx, 0xf00dcafe }};";
                else
                    os << "{{ idx, 0xf00dcafe, 0xdeadbeef, 0xbeeff00d }};";

                os << endl
                   << "union { "
                   << philoxName << "_ctr_t c; "
                   << unionIntType << vlen << " ii; "
                   << unionFloatType << vlen << " fp; } u;"
                   << endl
                   << "u.c = " << philoxName << "(rngCtr, rngKey);"
                   << endl
                   << "u.fp = convert_" << unionFloatType << vlen << "(u.ii)"
                      " * (" << unionFloatType << vlen << ")("
                   << (64 == prec ? _M_RAN_INVM64 : _M_RAN_INVM32)
                   << ") + ("
                   << unionFloatType << vlen << ")(0.5);"
                   << endl
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

                os << ";"
                   << endl
                   << "}"
                   << endl;
            }
        }
    }
}

void Random123::philoxDecl(ostream& os) const
{
    for (set< size_t >::const_iterator
         prec = _precSet.begin();
         prec != _precSet.end();
         prec++)
    {
        philoxMulhilo(os, *prec);

        os << endl;

        for (set< size_t >::const_iterator
             vlen = _vlenSet.begin();
             vlen != _vlenSet.end();
             vlen++)
        {
            philoxBumpkey(os, *prec, *vlen);

            os << endl;

            philoxRoundFun(os, *prec, *vlen);

            os << endl;

            philoxRoundEnum(os, *prec, *vlen);

            os << endl;

            philoxTypedef(os, *prec, *vlen);

            os << endl;

            philoxKeyinit(os, *prec, *vlen);

            os << endl;

            philox_R(os, *prec, *vlen);

            os << endl;

            philoxEntry(os, *prec, *vlen);

            os << endl;
        }
    }
}

void Random123::threefryEnum(ostream& os,
                             const size_t prec,
                             const size_t vlen) const
{
    os << "enum r123_enum_threefry" << prec << "x" << vlen
       << endl
       << "{"
       << endl;

    vector< string > enumKeys;
    vector< size_t > enumVals;

    switch (prec)
    {
        case (32) :
            switch (vlen)
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
            switch (vlen)
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
        os << "  R_" << prec << "x" << vlen << "_" << enumKeys[i]
           << "=" << enumVals[i];

        if (i < enumKeys.size()-1)
            os << ",";

        os << endl;
    }

    os << "};"
       << endl;
}

void Random123::threefryRotL(ostream& os,
                             const size_t prec) const
{
    stringstream ss;
    ss << "inline uint" << prec << "_t RotL_" << prec
       << "(uint" << prec << "_t x, unsigned int N)";
    const string funhead = ss.str();

    os << funhead << " __attribute__((always_inline));"
       << endl
       << funhead << " { return (x << (N & " << (prec-1) << ")) | (x >> (("
       << prec << "-N) & " << (prec-1) << ")); }"
       << endl;
}

void Random123::threefryTypedef(ostream& os,
                                const size_t prec,
                                const size_t vlen) const
{
    stringstream ss;
    ss << "typedef struct r123array" << vlen << "x" << prec
       << " threefry" << vlen << "x" << prec;

    os << ss.str() << "_ctr_t;"
       << endl
       << ss.str() << "_key_t;"
       << endl
       << ss.str() << "_ukey_t;"
       << endl;
}

void Random123::threefryKeyinit(ostream& os,
                                const size_t prec,
                                const size_t vlen) const
{
    stringstream ss;
    ss << "threefry" << vlen << "x" << prec;

    os << "inline " << ss.str() << "_key_t " << ss.str() << "keyinit("
       << ss.str() << "_ukey_t uk) { return uk; }"
       << endl;
}

void Random123::threefry_R(ostream& os,
                           const size_t prec,
                           const size_t vlen) const
{
    stringstream ss;

    ss << "threefry" << vlen << "x" << prec;
    const string threefryName = ss.str();

    ss.str(string());

    ss << "inline " << threefryName << "_ctr_t " << threefryName << "_R"
          "(unsigned int Nrounds, " << threefryName << "_ctr_t in, "
          << threefryName << "_key_t k)";
    const string funhead = ss.str();

    os << funhead << " __attribute((always_inline));"
       << endl
       << funhead
       << endl
       << "{"
       << endl;

    os << "  " << threefryName << "_ctr_t X;"
       << endl
       << "  uint" << prec << "_t ks[" << vlen << "+1];"
       << endl
       << "  int i;"
       << endl
       << "  ks[" << vlen << "] = ";

    switch (prec)
    {
        case (32) :
            os << "0x1BD11BDA";
            break;

        case (64) :
            os << "((0xA9FC1A22) + (((uint64_t) (0x1BD11BDA)) << 32))";
            break;
    }

    os << ";"
       << endl
       << "  for (i = 0; i < " << vlen << "; i++)"
       << endl
       << "  {"
       << endl
       << "    ks[i] = k.v[i];"
       << endl
       << "    X.v[i] = in.v[i];"
       << endl
       << "    ks[" << vlen << "] ^= k.v[i];"
       << endl
       << "  }"
       << endl;

    for (size_t i = 0; i < vlen; i++)
    {
        os << "  X.v[" << i << "] += ks[" << i << "];"
           << endl;
    }

    const size_t Nrounds = (2 == vlen) ? 32 : 72;
    const size_t resetRotL = (2 == vlen) ? 5 : -1;
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

            for (size_t k = 0; k < vlen; k+=2)
            {
                os << " X.v[" << k << "] += X.v[" << (k+1) << "];"
                      " X.v[" << (k+1) << "] ="
                      " RotL_" << prec << "(X.v[" << (k+1) << "],"
                      " R_" << prec << "x" << vlen
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

        for (size_t j = 0; j < vlen; j++)
        {
            os << " X.v[" << j << "] +="
                  " ks[" << ((countQuad+j) % (vlen+1)) << "];";
        }

        os << " X.v[" << (vlen-1) << "] += " << countQuad << "; }"
           << endl;
    }

    os << "  return X;"
       << endl
       << "}"
       << endl;
}

void Random123::threefryRoundEnum(ostream& os,
                                  const size_t prec,
                                  const size_t vlen) const
{
    os << "enum r123_enum_threefry" << vlen << "x" << prec
       << " { threefry" << vlen << "x" << prec << "_rounds = 20 };"
       << endl;
}

void Random123::threefryEntry(ostream& os,
                              const size_t prec,
                              const size_t vlen) const
{
    stringstream ss;

    ss << "threefry" << vlen << "x" << prec;
    const string threefryName = ss.str();

    ss.str(string());

    ss << "inline " << threefryName << "_ctr_t "
       << threefryName << "("
       << threefryName << "_ctr_t in, "
       << threefryName << "_key_t k)";
    const string funhead = ss.str();

    os << funhead << " __attribute((always_inline));"
       << endl
       << funhead << " { return " << threefryName << "_R(" << threefryName
       << "_rounds, in, k); }"
       << endl;

    map< size_t, string > wrapTypes; // keys come from PrecType sizeCode
    switch (prec)
    {
        case (32) :
            wrapTypes[0] = "uint";
            wrapTypes[1] = "int";
            wrapTypes[4] = "float";
            break;

        case (64) :
            wrapTypes[8] = "double";
            break;
    }

    map< size_t, string > wrapVectorLengths;
    switch (vlen)
    {
        case (2) :
            wrapVectorLengths[1] = "";
            wrapVectorLengths[2] = "2";
            break;

        case (4) :
            wrapVectorLengths[4] = "4";
            break;
    }

    const string baseUIntType = (64 == prec ? "uint64_t" : "uint32_t");
    const string unionUIntType = (64 == prec ? "ulong" : "uint");
    const string unionIntType = (64 == prec ? "long" : "int");
    const string unionFloatType = (64 == prec ? "double" : "float");

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

            os << "inline "
               << precTypePrimitiveName << vecLengthSuffix
               << " threefry_uniform_"
               << precTypeSizeCode << "_" << vecLength << "("
               << baseUIntType << " tid, "
               << baseUIntType << " idx) __attribute__((always_inline));"
               << endl
               << "inline "
               << precTypePrimitiveName << vecLengthSuffix
               << " threefry_uniform_"
               << precTypeSizeCode << "_" << vecLength << "("
               << baseUIntType << " tid, "
               << baseUIntType << " idx) {"
               << endl
               << "  " << threefryName << "_key_t rngKey = ";

            if (64 == prec)
                os << "{{ tid, 0xdecafbad }};";
            else
                os << "{{ tid, 0xdecafbad, 0xfacebead, 0x12345678 }};";

            os << endl
               << "  " << threefryName << "_ctr_t rngCtr = ";

            if (64 == prec)
                os << "{{ idx, 0xf00dcafe }};";
            else
                os << "{{ idx, 0xf00dcafe, 0xdeadbeef, 0xbeeff00d }};";

            os << endl
               << "union { "
               << threefryName << "_ctr_t c; "
               << unionUIntType << vlen << " ui; "
               << unionIntType << vlen << " ii; } u;"
               << endl
               << "u.c = " << threefryName << "(rngCtr, rngKey);"
               << endl
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
                       << (64 == prec ? _M_RAN_INVM64 : _M_RAN_INVM32)
                       << ") + ("
                       << precTypePrimitiveName << vecLengthSuffix
                       << ")(0.5)";
                    break;
            }

            os << ";"
               << endl
               << "}"
               << endl;

            // normal distribution (only makes sense for floating point)

            if (4 == precTypeSizeCode || 8 == precTypeSizeCode)
            {
                os << "inline "
                   << precTypePrimitiveName << vecLengthSuffix
                   << " threefry_normal_"
                   << precTypeSizeCode << "_" << vecLength << "("
                   << baseUIntType << " tid, "
                   << baseUIntType << " idx) __attribute__((always_inline));"
                   << endl
                   << "inline "
                   << precTypePrimitiveName << vecLengthSuffix
                   << " threefry_normal_"
                   << precTypeSizeCode << "_" << vecLength << "("
                   << baseUIntType << " tid, "
                   << baseUIntType << " idx) {"
                   << endl
                   << "  " << threefryName << "_key_t rngKey = ";

                if (64 == prec)
                    os << "{{ tid, 0xdecafbad }};";
                else
                    os << "{{ tid, 0xdecafbad, 0xfacebead, 0x12345678 }};";

                os << endl
                   << "  " << threefryName << "_ctr_t rngCtr = ";

                if (64 == prec)
                    os << "{{ idx, 0xf00dcafe }};";
                else
                    os << "{{ idx, 0xf00dcafe, 0xdeadbeef, 0xbeeff00d }};";

                os << endl
                   << "union { "
                   << threefryName << "_ctr_t c; "
                   << unionIntType << vlen << " ii; "
                   << unionFloatType << vlen << " fp; } u;"
                   << endl
                   << "u.c = " << threefryName << "(rngCtr, rngKey);"
                   << endl
                   << "u.fp = convert_" << unionFloatType << vlen << "(u.ii)"
                      " * (" << unionFloatType << vlen << ")("
                   << (64 == prec ? _M_RAN_INVM64 : _M_RAN_INVM32)
                   << ") + ("
                   << unionFloatType << vlen << ")(0.5);"
                   << endl
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

                os << ";"
                   << endl
                   << "}"
                   << endl;
            }
        }
    }
}

void Random123::threefryDecl(ostream& os) const
{
    for (set< size_t >::const_iterator
         prec = _precSet.begin();
         prec != _precSet.end();
         prec++)
    {
        threefryRotL(os, *prec);

        os << endl;

        for (set< size_t >::const_iterator
             vlen = _vlenSet.begin();
             vlen != _vlenSet.end();
             vlen++)
        {
            threefryEnum(os, *prec, *vlen);

            os << endl;

            threefryTypedef(os, *prec, *vlen);

            os << endl;

            threefryKeyinit(os, *prec, *vlen);

            os << endl;

            threefry_R(os, *prec, *vlen);

            os << endl;

            threefryRoundEnum(os, *prec, *vlen);

            os << endl;

            threefryEntry(os, *prec, *vlen);

            os << endl;
        }
    }
}

Random123::Random123(void)
    : _M_RAN_INVM32("2.328306436538696289063E-10"),
      _M_RAN_INVM64("5.421010862427522170037E-20"),
      _precSet(),
      _vlenSet(),
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

void Random123::vlen2(void)
{
    _vlenSet.insert(2);
}

void Random123::vlen4(void)
{
    _vlenSet.insert(4);
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
        philoxDecl(os);
    }

    // threefry RNG
    if (_threefry)
    {
        threefryDecl(os);
    }
}

}; // namespace chai_internal
