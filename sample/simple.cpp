#include <iostream>
#include <peakstream.h>

using namespace chai;
using namespace std;

int main(int argc, char *argv[])
{
    init(); // initialize virtual machine

    // buffers on CPU
    double cpuA[100], cpuB[100], cpuC[100];
    for (size_t i = 0; i < 100; i++)
    {
        cpuA[i] = i;
        cpuB[i] = 99 - i;
    }

    { // force array destructors before shutdown

    Arrayf64 C;
    {
        Arrayf64 A = make1(100, cpuA);
        Arrayf64 B = make1(100, cpuB);

        C = A + B;
    }
    C.read1(cpuC, 100 * sizeof(double));

    } // force array destructors before shutdown

    shutdown(); // shutdown virtual machine

/*
    for (size_t i = 0; i < 100; i++)
    {
        cout << "cpuC[" << i << "] = " << cpuC[i] << endl;
    }
*/
    double accum = 0;
    for (size_t i = 0; i < 100; i++)
    {
        accum += cpuC[i];
    }
    cout << accum << endl;

    return 0;
}
