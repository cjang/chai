#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <cstdlib>
#include <iostream>

using namespace chai;
using namespace std;

//
// Simple calculation using the Chai domain specific language
//

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).initVM();

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
    // calculate saxpy: Y = alpha * X + Y

    Y += alpha * X;

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
