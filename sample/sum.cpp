#include <iostream>
#include <peakstream.h>

using namespace chai;
using namespace std;

int main(int argc, char *argv[])
{
    init(); // initialize virtual machine

    // buffers on CPU
    double cpuA[100], cpuB[100];
    for (size_t i = 0; i < 100; i++) { cpuA[i] = i; cpuB[i] = 99 - i; }

    double c;

    { // force array destructors before shutdown

    Arrayf64 C;
    {
        // eagerly create buffers/images on compute device
        Arrayf64 A = make1(100, cpuA);
        Arrayf64 B = make1(100, cpuB);

        // lazy boxed calculation
        C = sum(A + B);
    }
    c = C.read_scalar(); // force evaluation, read back

    } // force array destructors before shutdown

    shutdown(); // shutdown virtual machine

    cout << "result is " << c << endl; // result should be 9900

    return 0;
}
