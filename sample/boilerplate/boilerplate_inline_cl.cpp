#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <cstdlib>
#include <iostream>

using namespace chai;
using namespace std;

//
// Simple calculation using inlined OpenCL and Chai runtime management
//

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).noInterpret().initVM();

    ////////////////////////////////////////
    // initialize data buffers on host CPU

    size_t N = 20;
    float cpuX[N], cpuY[N]; // N x 1 row major array buffers

    // initialize array data
    for (size_t i = 0; i < N; i++)
    {
        cpuX[i] = i;
        cpuY[i] = 1;
    }

    float alpha = 1.5f;

    ////////////////////////////////////////
    // create managed arrays

    Arrayf32 X = make1(N, cpuX);
    Arrayf32 Y = make1(N, cpuY);

    ////////////////////////////////////////
    // inlined OpenCL kernel

    // saxpy: Y = alpha * X + Y
    ProgramCL inlineCL(
        "__kernel void saxpy(const float alpha,"
        "                    __global const float* X,"
        "                    __global float* Y)"
        "{"
        "    Y[get_global_id(0)] += alpha * X[get_global_id(0)];"
        "}" );

    ////////////////////////////////////////
    // run the kernel

    const size_t groupsize = 4;
    (inlineCL, "saxpy", alpha, X, Y)(N, groupsize);

    ////////////////////////////////////////
    // evaluate and read back result

    Y.read1(cpuY, N * sizeof(float));

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    for (size_t i = 0; i < N; i++)
        cout << "Y[" << i << "] is " << cpuY[i] << endl;

    exit(0);
}
