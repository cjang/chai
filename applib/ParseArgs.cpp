// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

#include "chai/chai.h"
#include "EvergreenMatmulBase.hpp"
#include "EvergreenMatmulMM.hpp"
#include "EvergreenMatmulMV.hpp"
#include "OCLhacks.hpp"
#include "OCLinit.hpp"
#include "StandardEM.hpp"
#include "ParseArgs.hpp"

using namespace chai_internal;
using namespace std;

namespace chai {

////////////////////////////////////////
// parse command line arguments

void ParseArgs::setVectorLength(const char tok,
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

void ParseArgs::setVectorLengthLimits(const size_t vectorLength,
                                      const bool useImages,
                                      size_t& startIdx,
                                      size_t& stopIdx) const
{
    const size_t vlen0 = useImages
                             ? 0  // use images
                             : 1; // no image support

    switch (vectorLength)
    {
        // all vector lengths, images and memory buffers
        case (-1) :
            startIdx = vlen0;
            stopIdx = 4;
            break;

        // memory buffers only
        case (7) :
            startIdx = 1; // no image support
            stopIdx = 4;
            break;

        // fixed to specified vector length
        default :
            startIdx = stopIdx = vectorLength;
            break;
    }
}

ParseArgs::ParseArgs(int argc, char *argv[])
    : _argc(argc),
      _argv(argv),
      _deviceIdx(-1),
      _packing(-1),
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
      _includeReadDataFromDevice(false) { }

bool ParseArgs::initVM(void)
{
    bool ok = false;

    string configSpec;

    int opt;
    while (-1 != (opt = getopt(_argc, _argv, "f:")))
    {
        switch (opt)
        {
            // -f configspec_filename
            case ('f') :
                configSpec = optarg;
                break;
        }
    }

    if (! configSpec.empty())
    {
        chai::init(configSpec); // part of external API, initialize runtime
        ok = true;
    }

    return ok;
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

            // -c packing_count
            case ('c') :
                {
                stringstream ss(optarg);
                ss >> _packing;
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

            // -p SSS|SSD|SDS...DDD
            case ('p') :
                {
                const string t = optarg;
                if (3 == t.size())
                {
                    _precA = 'D' == t[0] ? sizeof(double) : sizeof(float);
                    _precB = 'D' == t[1] ? sizeof(double) : sizeof(float);
                    _precC = 'D' == t[2] ? sizeof(double) : sizeof(float);
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
                    setVectorLength(t[0], _vecLenA);
                    setVectorLength(t[1], _vecLenB);
                    setVectorLength(t[2], _vecLenC);
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
    if (-1 != _packing)
        kernelGen.setPacking(_packing);

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
    if (-1 != _packing)
        kernelGen.setPacking(_packing);

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

    setVectorLengthLimits(kernelGen.vectorLengthA(), useImg, startA, stopA);
    setVectorLengthLimits(kernelGen.vectorLengthB(), useImg, startB, stopB);
    setVectorLengthLimits(kernelGen.vectorLengthC(), useImg, startC, stopC);
}

void ParseArgs::setOpt(StandardEM& stdEM)
{
    if (_includeSendDataToDevice)
        stdEM.setIncludeSendDataToDevice(_includeSendDataToDevice);

    if (_includeReadDataFromDevice)
        stdEM.setIncludeReadDataFromDevice(_includeReadDataFromDevice);
}

}; // namespace chai
