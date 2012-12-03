#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <cstdlib>
#include <iostream>

using namespace chai;
using namespace std;

//
// Bitonic sort using inlined OpenCL taken from AMD-APP-SDK-v2.6-RC3-lnx64
// sample code.
//
// The ".cl" file is copied from samples/opencl/cl/app/BitonicSort/ . This
// file is unusued but included for reference and to retain the orignal
// copyright of Advanced Micro Devices.
//

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).noInterpret().initVM();

    ////////////////////////////////////////
    // initialize data buffers on host CPU

    const size_t N = 16; // must be even power of two
    uint32_t cpuA[N];

    for (size_t i = 0; i < N; i++)
        cpuA[i] = random();

    ////////////////////////////////////////
    // create managed array

    Arrayu32 A = make1(N, cpuA);

    ////////////////////////////////////////
    // managed code: bitonic sorting network

    // one pass in a stage of the sorting network
    ProgramCL inlineCL(
        "__kernel "
        "void bitonicSort(__global uint * theArray,"
        "                 const uint stage,"
        "                 const uint passOfStage,"
        "                 const uint width,"
        "                 const uint direction)"
        "{"
        "    uint sortIncreasing = direction;"
        "    uint threadId = get_global_id(0);"
        "" 
        "    uint pairDistance = 1 << (stage - passOfStage);"
        "    uint blockWidth   = 2 * pairDistance;"
        ""
        "    uint leftId = (threadId % pairDistance)"
        "                   + (threadId / pairDistance) * blockWidth;"
        ""
        "    uint rightId = leftId + pairDistance;"
        ""
        "    uint leftElement = theArray[leftId];"
        "    uint rightElement = theArray[rightId];"
        ""
        "    uint sameDirectionBlockWidth = 1 << stage;"
        ""
        "    if((threadId/sameDirectionBlockWidth) % 2 == 1)"
        "        sortIncreasing = 1 - sortIncreasing;"
        ""
        "    uint greater;"
        "    uint lesser;"
        "    if(leftElement > rightElement)"
        "    {"
        "        greater = leftElement;"
        "        lesser  = rightElement;"
        "    }"
        "    else"
        "    {"
        "        greater = rightElement;"
        "        lesser  = leftElement;"
        "    }"
        ""
        "    if(sortIncreasing)"
        "    {"
        "        theArray[leftId]  = lesser;"
        "        theArray[rightId] = greater;"
        "    }"
        "    else"
        "    {"
        "        theArray[leftId]  = greater;"
        "        theArray[rightId] = lesser;"
        "    }"
        "}" );

    // determine number of stages: width = pow(2, numStages)
    size_t numStages = 0;
    for (uint32_t temp = N; temp > 1; temp >>= 1)
        numStages++;

    // stages of the algorithm
    for (uint32_t stage = 0; stage < numStages; stage++)
    {
        // passes of current stage
        for (uint32_t passOfStage = 0; passOfStage < stage + 1; passOfStage++)
        {
            (inlineCL,
             "bitonicSort",
             A,
             stage,
             passOfStage,
             N,
             1)(N/2, 1);
        }
    }

    ////////////////////////////////////////
    // read back result

    A.read1(cpuA, N*sizeof(uint32_t));

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    for (size_t i = 0; i < N; i++)
        cout << "[" << i << "] " << cpuA[i] << endl;

    exit(0);
}
