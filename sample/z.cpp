#include <iostream>
#include <peakstream.h>

using namespace chai;
using namespace std;

int main(int argc, char *argv[])
{
    init(); // initialize virtual machine
    log(cout);
    mass(100);

    // buffers on CPU
    float cpuA[100], cpuB[100];
    for (size_t i = 0; i < 100; i++) { cpuA[i] = i; cpuB[i] = 99 - i; }

    Arrayf32 C;
    {
        // eagerly create buffers/images on compute device
        Arrayf32 A = make1(100, cpuA);
        Arrayf32 B = make1(100, cpuB);

        // lazy boxed calculation
        C = sqrt(B + sum(A + B * mean(A)));
    }

    float cpuC[100];
    C.read1(cpuC, 100 * sizeof(float)); // force evaluation, read back

    shutdown(); // shutdown virtual machine

    for (size_t i = 0; i < 100; i++)
        cout << cpuC[i] << " ";
    cout << endl;

    return 0;
}
