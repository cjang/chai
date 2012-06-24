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

const size_t N = 40;                      // array dimensions

double cpuA[NUMBER_PARALLEL][N * N],      // buffers on CPU
       cpuB[NUMBER_PARALLEL][N * N],
       cpuC[NUMBER_PARALLEL][N * N];

double result[NUMBER_PARALLEL];           // output

const double alpha = .777f;
const double beta = .888f;

///////////////////////////////////////////
// serial example, one thread in a loop

#ifdef _SAMPLE_USE_ONETHREAD_
double managedCode(const size_t i)
{
    Arrayf64 D;
    {
        Arrayf64 A = make2(N, N, cpuA[i]);
        Arrayf64 B = make2(N, N, cpuB[i]);
        Arrayf64 C = make2(N, N, cpuC[i]);

        D = sum(matmul(A, B, C, alpha, beta));
    }
    return D.read_scalar();
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
vector< double > managedCode(const vector< double* >& dataA,
                             const vector< double* >& dataB,
                             const vector< double* >& dataC)
{
    Arrayf64 D;
    {
        Arrayf64 A = make2(N, N, dataA);
        Arrayf64 B = make2(N, N, dataB);
        Arrayf64 C = make2(N, N, dataC);

        D = sum(matmul(A, B, C, alpha, beta));
    }
    return D.read_scalar(NUMBER_PARALLEL);
}

void body(void)
{
    vector< double* > dataA, dataB, dataC;
    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
    {
        dataA.push_back(cpuA[i]);
        dataB.push_back(cpuB[i]);
        dataC.push_back(cpuC[i]);
    }

    const vector< double > resultvec = managedCode(dataA, dataB, dataC);

    for (size_t i = 0; i < NUMBER_PARALLEL; i++)
        result[i] = resultvec[i];
}
#endif

///////////////////////////////////////////
// concurrent/parallel using OpenMP

#ifdef _SAMPLE_USE_OPENMP_
double managedCode(const size_t i)
{
    Arrayf64 D;
    {
        Arrayf64 A = make2(N, N, cpuA[i]);
        Arrayf64 B = make2(N, N, cpuB[i]);
        Arrayf64 C = make2(N, N, cpuC[i]);

        D = sum(matmul(A, B, C, alpha, beta));
    }
    return D.read_scalar();
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
double managedCode(const size_t i)
{
    Arrayf64 D;
    {
        Arrayf64 A = make2(N, N, cpuA[i]);
        Arrayf64 B = make2(N, N, cpuB[i]);
        Arrayf64 C = make2(N, N, cpuC[i]);

        D = sum(matmul(A, B, C, alpha, beta));
    }
    return D.read_scalar();
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
        for (size_t j = 0; j < N * N; j++)
        {
            cpuA[i][j] = 0.001f * j;
            cpuB[i][j] = 0.001f * (N * N - j);
            cpuC[i][j] = 1.0f + i;
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
