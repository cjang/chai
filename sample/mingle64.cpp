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

    ParseArgs(argc, argv).initVM(); // start virtual machine, exit on error

    /////////////////////////////////////
    // computational work

    // input data
    double cpuA[10], cpuB[10];
    for (size_t i = 0; i < 10; i++)
        cpuA[i] = i;

    // loop is unmanaged
    for (size_t i = 0; i < 5; i++)
    {
        // managed code
        Arrayf64 B;
        {
            Arrayf64 A = make1(10, cpuA);
            B = exp(A);
        }
        B.read1(cpuB, 10 * sizeof(double));

        // unmanaged code
        cpuA[i] += cos(cpuB[2*i]) + i;
    }

    // print computed result
    for (size_t i = 0; i < 10; i++)
        cout << "cpuB[" << i << "] is " << cpuB[i] << endl;

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown(); // shutdown virtual machine

    exit(0);
}
