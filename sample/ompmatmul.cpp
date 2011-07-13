#include <iostream>
#include <map>
#include <omp.h>
#include <peakstream.h>
#include <set>
#include <vector>

using namespace chai;
using namespace std;

int main(int argc, char *argv[])
{
    init(); // initialize virtual machine

    const size_t N = 10;

    double cpuA[N*N], cpuB[N*N];

    #pragma omp parallel for
    for (size_t i = 0; i < 100; i++)
    {
        //double cpuA[N*N], cpuB[N*N];
        Arrayf64 A = make2(N, N, cpuA);
        Arrayf64 B = make2(N, N, cpuB);
        Arrayf64 C = matmul(A, B);

        double cpuC[N*N];

        C.read1(cpuC, N*N);
    }

    shutdown(); // shutdown virtual machine

    return 0;
}
