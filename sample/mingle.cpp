#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace chai;
using namespace std;

int main(int argc, char *argv[])
{
    /////////////////////////////////////
    // boilerplate: start virtual machine

    ParseArgs pargs(argc, argv);
    if (! pargs.initVM()) // initialize virtual machine
    {
        cerr << "usage: " << argv[0] << " -f configspec" << endl;
        exit(1);
    }

    /////////////////////////////////////
    // computational work

    // input data
    float cpuA[10], cpuB[10];
    for (size_t i = 0; i < 10; i++)
        cpuA[i] = i;

    // loop is unmanaged
    for (size_t i = 0; i < 5; i++)
    {
        // managed code
        Arrayf32 B;
        {
            Arrayf32 A = make1(10, cpuA);
            B = exp(A);
        }
        B.read1(cpuB, 10 * sizeof(float));

        // unmanaged code
        cpuB[i] += cos(cpuB[2*i]) + i;
    }

    // print computed result
    for (size_t i = 0; i < 10; i++)
        cout << "cpuB[" << i << "] is " << cpuB[i] << endl;

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown(); // shutdown virtual machine

    exit(0);
}
