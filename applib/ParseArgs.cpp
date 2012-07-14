// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>

#include "chai/chai.h"
#include "EvergreenMatmulBase.hpp"
#include "EvergreenMatmulMM.hpp"
#include "EvergreenMatmulMV.hpp"
#include "OCLhacks.hpp"
#include "OCLinit.hpp"
#include "PrecType.hpp"
#include "StandardEM.hpp"
#include "ParseArgs.hpp" // last included file due to pre-processor macros

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// parse command line arguments

void ParseArgs::setVecLength(const char tok,
                             size_t& vecLen) const
{
    switch (tok)
    {
        case ('0') : vecLen = 0; break;
        case ('1') : vecLen = 1; break;
        case ('2') : vecLen = 2; break;
        case ('4') : vecLen = 4; break;
    }
}

void ParseArgs::setVecLengthLimits(const size_t vecLen,
                                   const bool useImages,
                                   size_t& startIdx,
                                   size_t& stopIdx) const
{
    switch (vecLen)
    {
        // all vector lengths, images and memory buffers
        case (-1) :
            startIdx = useImages
                           ? 0  // use images
                           : 1; // no image support
            stopIdx = 4;
            break;

        // memory buffers only
        case (7) :
            startIdx = 1; // no image support
            stopIdx = 4;
            break;

        // fixed to specified vector length
        default :
            startIdx = stopIdx = vecLen;
            break;
    }
}

size_t ParseArgs::precChar(const char UISD) const
{
    switch (UISD)
    {
        case ('U') :
            return PrecType::UInt32;

        case ('I') :
            return PrecType::Int32;

        case ('S') :
            return PrecType::Float;

        case ('D') :
            return PrecType::Double;

        default :
            return -1;
    }
}

ParseArgs::ParseArgs(int argc, char *argv[])
    : _argc(argc),
      _argv(argv),
      _deviceIdx(-1),
      _batching(-1),
      _general(-1),
      _precA(-1),
      _precB(-1),
      _precC(-1),
      _vecLenA(-1),
      _vecLenB(-1),
      _vecLenC(-1),
      _dimM(-1),
      _dimN(-1),
      _dimK(-1),
      _transA(-1),
      _transB(-1),
      _includeSendDataToDevice(false),
      _includeReadDataFromDevice(false),
      _noInterpret(false) { }

ParseArgs& ParseArgs::noInterpret(void)
{
    _noInterpret = true;

    return *this;
}

void ParseArgs::initVM(void)
{
    bool ok = false;

    int opt;
    while (-1 != (opt = getopt(_argc, _argv, "f:i")))
    {
        switch (opt)
        {
            // -f configspec_filename
            case ('f') :
                if (strlen(optarg))
                {
                    // part of external API, initialize runtime
                    chai::init(optarg);
                    ok = true;
                }
                break;

            // -i
            case ('i') :
                if (! _noInterpret)
                {
                    chai::init(""); // interpreter active by default
                    ok = true;
                }
                break;
        }
    }

    if (! ok)
    {
        cerr << "usage: " << _argv[0] << " [-f configspec]";

        if (! _noInterpret)
            cerr << " [-i]";

        cerr << endl;

        exit(1);
    }
}

bool ParseArgs::getOpt(OCLinit& cinit)
{
    bool ok = true;

    int opt;
    while (-1 != (opt = getopt(_argc, _argv, "f:c:gGp:v:m:n:k:t:sr")))
    {
        switch (opt)
        {
            // -f configspec_filename
            case ('f') :
                {
                ifstream configSpec(optarg);
                OCLhacks::singleton().initHacks(cinit, configSpec);
                }
                break;

            // -c batching_count
            case ('c') :
                {
                stringstream ss(optarg);
                ss >> _batching;
                if (!ss) ok = false;
                }
                break;

            // -g
            case ('g') :
                _general = 0; // false
                break;

            // -G
            case ('G') :
                _general = 1; // true
                break;

            // -p [UISD]{3}
            case ('p') :
                {
                const string t = optarg;
                if (3 == t.size())
                {
                    if (-1 == (_precA = precChar(t[0]))) ok = false;
                    if (-1 == (_precB = precChar(t[1]))) ok = false;
                    if (-1 == (_precC = precChar(t[2]))) ok = false;
                }
                else
                    ok = false;
                }
                break;

            // -v 000|001|002...444|2x4|777
            case ('v') :
                {
                const string t = optarg;
                if (3 == t.size())
                {
                    setVecLength(t[0], _vecLenA);
                    setVecLength(t[1], _vecLenB);
                    setVecLength(t[2], _vecLenC);
                }
                else
                    ok = false;
                }
                break;

            // -m M_dimension
            case ('m') :
                {
                stringstream ss(optarg);
                ss >> _dimM;
                if (!ss) ok = false;
                }
                break;

            // -n N_dimension
            case ('n') :
                {
                stringstream ss(optarg);
                ss >> _dimN;
                if (!ss) ok = false;
                }
                break;

            // -k K_dimension
            case ('k') :
                {
                stringstream ss(optarg);
                ss >> _dimK;
                if (!ss) ok = false;
                }
                break;

            // -t NN|NT|TN|TT|N|T
            case ('t') :
                {
                const string t = optarg;
                if ("NN" == t) { _transA = 0; _transB = 0; }
                else if ("NT" == t) { _transA = 0; _transB = 1; }
                else if ("TN" == t) { _transA = 1; _transB = 0; }
                else if ("TT" == t) { _transA = 1; _transB = 1; }
                else if ("N" == t) { _transA = 0; }
                else if ("T" == t) { _transA = 1; }
                }
                break;

            // -s
            case ('s') :
                _includeSendDataToDevice = true;
                break;

            // -r
            case ('r') :
                _includeReadDataFromDevice = true;
                break;
        }
    }

    _deviceIdx = OCLhacks::singleton().firstEvergreen();

    return ok && (-1 != _deviceIdx);
}

size_t ParseArgs::getDeviceIndex(void) const
{
    return _deviceIdx;
}

void ParseArgs::setOpt(Evergreen::MatmulMMBase& kernelGen)
{
    if (-1 != _batching)
        kernelGen.setBatching(_batching);

    if (-1 != _general)
        kernelGen.setGeneral(_general);

    if (-1 != _precA && -1 != _precB && -1 != _precC)
        kernelGen.setPrecision(_precA, _precB, _precC);

    kernelGen.setVectorLength(_vecLenA, _vecLenB, _vecLenC);

    if (-1 != _dimM && -1 == _dimN && -1 == _dimK)
        kernelGen.setDimensions(_dimM, _dimM, _dimM);
    else if (-1 == _dimM && -1 != _dimN && -1 == _dimK)
        kernelGen.setDimensions(_dimN, _dimN, _dimN);
    else if (-1 == _dimM && -1 == _dimN && -1 != _dimK)
        kernelGen.setDimensions(_dimK, _dimK, _dimK);
    else if (-1 != _dimM && -1 != _dimN && -1 != _dimK)
        kernelGen.setDimensions(_dimM, _dimN, _dimK);

    if (-1 != _transA && -1 != _transB)
        kernelGen.setDataLayout(_transA, _transB);
}

void ParseArgs::setOpt(Evergreen::MatmulMVBase& kernelGen)
{
    if (-1 != _batching)
        kernelGen.setBatching(_batching);

    if (-1 != _general)
        kernelGen.setGeneral(_general);

    if (-1 != _precA && -1 != _precB && -1 != _precC)
        kernelGen.setPrecision(_precA, _precB, _precC);

    kernelGen.setVectorLength(_vecLenA, _vecLenB, _vecLenC);

    if (-1 != _dimM && -1 == _dimN && -1 == _dimK)
        kernelGen.setDimensions(_dimM, _dimM);
    else if (-1 == _dimM && -1 != _dimN && -1 == _dimK)
        kernelGen.setDimensions(_dimN, _dimN);
    else if (-1 != _dimM && -1 != _dimN && -1 == _dimK)
        kernelGen.setDimensions(_dimM, _dimN);

    if (-1 != _transA && -1 == _transB)
        kernelGen.setDataLayout(_transA);
}

void ParseArgs::setOpt(Evergreen::MatmulBase& kernelGen,
                       size_t& startA,
                       size_t& startB,
                       size_t& startC,
                       size_t& stopA,
                       size_t& stopB,
                       size_t& stopC)
{
    const bool useImg = OCLhacks::singleton().supportImages(_deviceIdx);

    setVecLengthLimits(kernelGen.vecLengthA(), useImg, startA, stopA);
    setVecLengthLimits(kernelGen.vecLengthB(), useImg, startB, stopB);
    setVecLengthLimits(kernelGen.vecLengthC(), useImg, startC, stopC);
}

void ParseArgs::setOpt(StandardEM& stdEM)
{
    if (_includeSendDataToDevice)
        stdEM.setIncludeSendDataToDevice(_includeSendDataToDevice);

    if (_includeReadDataFromDevice)
        stdEM.setIncludeReadDataFromDevice(_includeReadDataFromDevice);
}

}; // namespace chai
