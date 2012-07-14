#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>

using namespace chai;
using namespace std;

void body(void)
{
    ProgramCL progCL(
        "__kernel void add_scalar(__global float* a, float s)"
        "{"
        "  a[get_global_id(0)] += s;"
        "}" );

    const size_t N = 20;

    float cpuA[N], cpuB[N], cpuC[N];
    for (size_t i = 0; i < N; i++)
    {
        cpuA[i] = i;
        cpuB[i] = N - i;
    }

    Arrayf32 C;
    {
        Arrayf32 A = make1(N, cpuA);
        Arrayf32 B = make1(N, cpuB);

        C = A + B;

        (progCL, "add_scalar", C, 1.5f)(N, 1);

        C += A - 3.3f;
    }
    C.read1(cpuC, N * sizeof(float));

    for (size_t i = 0; i < N; i++)
    {
        cout << "C[" << i << "] is " << cpuC[i] << endl;
    }
}

int main(int argc, char *argv[])
{
    /////////////////////////////////////
    // boilerplate: start virtual machine

    // start virtual machine, exit on error
    ParseArgs(argc, argv).noInterpret().initVM();

    /////////////////////////////////////
    // computational work

    body();

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown();

    exit(0);
}
