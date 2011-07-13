#include <iostream>
#include <peakstream.h>

using namespace chai;
using namespace std;

int main(int argc, char *argv[])
{
    init(); // initialize virtual machine

    // buffers on CPU
    double cpuA[100];
    for (size_t i = 0; i < 100; i++) { cpuA[i] = i; }

    double c;

    { // force array destructors before shutdown

        Arrayf64 C;

        { // limit readout variables to C

        Arrayf64 A = make1(100, cpuA);
        Arrayf64 B = 5;

        for (size_t i = 0; i < 2; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                A = A + mean(B);
            }

            //B = B + 1; // this way works too
            B += 1;
        }

        C = sum(A);

        } // limit readout variables to C

        c = C.read_scalar(); // force evaluation, read back

    } // force array destructors before shutdown

    shutdown(); // shutdown virtual machine

    cout << "result is " << c << endl;

    return 0;
}
