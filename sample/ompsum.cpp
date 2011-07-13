#include <iostream>
#include <omp.h>
#include <peakstream.h>
#include <pthread.h>

using namespace chai;
using namespace std;

double cpuA[100], cpuB[100];

void* calc(void* ctx)
{
    Arrayf64 A = make1(100, cpuA);
    Arrayf64 B = make1(100, cpuB);
    Arrayf64 C = sum(A + B);
    double c = C.read_scalar();

//    cerr << "pthread_self" << pthread_self()
//         << "\tread_scalar " << c << endl;

    return NULL;
}

int main(int argc, char *argv[])
{
    init(); // initialize virtual machine

    for (size_t i = 0; i < 100; i++)
    {
        cpuA[i] = i;
        cpuB[i] = 99 - i;
    }

//    cerr << "omp_get_thread_limit() = " << omp_get_thread_limit() << endl;

    #pragma omp parallel for
    for (size_t i = 0; i < 250; i++)
    {
        calc(NULL);
/*
        Arrayf64 A = make1(100, cpuA);
        Arrayf64 B = make1(100, cpuB);
        Arrayf64 C = sum(A + B);
        double c = C.read_scalar();
*/
    }

    shutdown(); // shutdown virtual machine

    return 0;
}
