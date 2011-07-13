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
    float cpuA[5], cpuB[5], cpuC[25];
    for (size_t i = 0; i < 5; i++) { cpuA[i] = i+1; cpuB[i] = i+1; }

    Arrayf32 C;
    {
        // eagerly create buffers/images on compute device
        Arrayf32 A = make1(5, cpuA);
        Arrayf32 B = make1(5, cpuB);

        // lazy boxed calculation
        //C = matmul(A, B);
        C = matmul(A, B);
    }
    C.read2(cpuC, 25 * sizeof(float));

    shutdown(); // shutdown virtual machine

    for (size_t j = 0; j < 25; j++)
        cout << cpuC[j] << " ";
    cout << endl;

    return 0;
}
