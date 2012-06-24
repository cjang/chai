#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <omp.h>
#include <pthread.h>
#include <stdlib.h>
#include <vector>

using namespace chai;
using namespace std;

///////////////////////////////////////////
// data

const size_t NUMBER_PARALLEL = 10;        // number iterations or concurrency

const size_t ARRAY_SIZE = 100;            // array dimensions

float cpuA[NUMBER_PARALLEL][ARRAY_SIZE],  // buffers on CPU
      cpuB[NUMBER_PARALLEL][ARRAY_SIZE];

float result[NUMBER_PARALLEL];            // output

///////////////////////////////////////////
// serial example, one thread in a loop

#ifdef _SAMPLE_USE_ONETHREAD_
float managedCode(const size_t i)
{
    Arrayf32 C;
    {
        Arrayf32 A = make1(ARRAY_SIZE, cpuA[i]);
        Arrayf32 B = make1(ARRAY_SIZE, cpuB[i]);

        C = sum(A + B);
    }
    return C.read_scalar();
}

void body(void)
{
    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
        result[i] = managedCode(i);
}
#endif

///////////////////////////////////////////
// data parallel using vectors

#ifdef _SAMPLE_USE_VECTOR_
vector< float > managedCode(const vector< float* >& dataA,
                            const vector< float* >& dataB)
{
    Arrayf32 C;
    {
        Arrayf32 A = make1(ARRAY_SIZE, dataA);
        Arrayf32 B = make1(ARRAY_SIZE, dataB);

        C = sum(A + B);
    }
    return C.read_scalar(NUMBER_PARALLEL);
}

void body(void)
{
    vector< float* > dataA, dataB;
    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
    {
        dataA.push_back(cpuA[i]);
        dataB.push_back(cpuB[i]);
    }

    const vector< float > resultvec = managedCode(dataA, dataB);

    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
        result[i] = resultvec[i];
}
#endif

///////////////////////////////////////////
// concurrent/parallel using OpenMP

#ifdef _SAMPLE_USE_OPENMP_
float managedCode(const size_t i)
{
    Arrayf32 C;
    {
        Arrayf32 A = make1(ARRAY_SIZE, cpuA[i]);
        Arrayf32 B = make1(ARRAY_SIZE, cpuB[i]);

        C = sum(A + B);
    }
    return C.read_scalar();
}

void body(void)
{
    // override OpenMP default of threads matching number of cores
    omp_set_num_threads(NUMBER_PARALLEL);

    // launch threads
    #pragma omp parallel for
    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
        result[i] = managedCode(i);
}
#endif

///////////////////////////////////////////
// concurrent/parallel using PThreads

#ifdef _SAMPLE_USE_PTHREADS_
float managedCode(const size_t i)
{
    Arrayf32 C;
    {
        Arrayf32 A = make1(ARRAY_SIZE, cpuA[i]);
        Arrayf32 B = make1(ARRAY_SIZE, cpuB[i]);

        C = sum(A + B);
    }
    return C.read_scalar();
}

void* pthWrapper(void* ctx)
{
    const size_t i = reinterpret_cast< size_t >(ctx);

    result[i] = managedCode(i);

    return NULL;
}

void body(void)
{
    // launch threads
    pthread_t pth[NUMBER_PARALLEL];
    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
        pthread_create(&pth[i],
                       NULL,
                       pthWrapper,
                       reinterpret_cast< void* >(i));

    // wait for threads
    void* status;
    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
        pthread_join(pth[i],
                     &status);
}
#endif

///////////////////////////////////////////
// main

int main(int argc, char *argv[])
{
    /////////////////////////////////////
    // initialize input data

    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
    {
        for (size_t j = 0; j < ARRAY_SIZE; j++)
        {
            cpuA[i][j] = j + i;
            cpuB[i][j] = ARRAY_SIZE - j;
        }
    }

    /////////////////////////////////////
    // boilerplate: start virtual machine

    ParseArgs(argc, argv).initVM(); // start virtual machine, exit on error

    /////////////////////////////////////
    // computational work

    body();

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown(); // shutdown virtual machine

    /////////////////////////////////////
    // print output result

    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
        cout << "result[" << i << "] is " << result[i] << endl;

    exit(0);
}
