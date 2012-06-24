#include <chai/chai.h>
#include <chai/chaiExt.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>

using namespace chai;
using namespace std;

void body(void)
{
    // buffers on CPU
    double cpuA[100], cpuB[100];
    for (size_t i = 0; i < 100; i++)
    {
        cpuA[i] = i;
        cpuB[i] = 99 - i;
    }

    Arrayf64 C;
    {
        Arrayf64 A = make1(100, cpuA);
        Arrayf64 B = make1(100, cpuB);

        C = sum(A + nop(B));
    }
    const double c = C.read_scalar();

    cout << "result is " << c << endl;
}

int main(int argc, char *argv[])
{
    ParseArgs(argc, argv).initVM(); // start virtual machine, exit on error

    initExt(); // language extensions

    body();

    shutdown(); // shutdown virtual machine

    exit(0);
}
