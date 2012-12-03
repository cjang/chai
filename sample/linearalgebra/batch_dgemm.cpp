#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace chai;
using namespace std;

//
// Parallel general matrix multiplication and reduction of matrix products
// in double precision
//
// 1. create PARALLEL managed arrays for A, B, C
// 2. calculate C = alpha * A * B + beta * C in parallel (note: matrix matrix)
// 3. calculate sum(C) in parallel
// 4. read back PARALLEL results
//

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).initVM();

    ////////////////////////////////////////
    // initialize data buffers on host CPU

    const size_t PARALLEL = 10;
    const size_t N = 40;

    // array buffers
    double cpuA[PARALLEL][N * N];
    double cpuB[PARALLEL][N * N];
    double cpuC[PARALLEL][N * N];

    // batched arrays
    vector< double* > dataA, dataB, dataC;

    // initialize arrays
    for (size_t i = 0; i < PARALLEL; i++)
    {
        // fill arrays with random numbers
        for (size_t j = 0; j < N * N; j++)
        {
            cpuA[i][j] = static_cast<double>(random()) / RAND_MAX;
            cpuB[i][j] = static_cast<double>(random()) / RAND_MAX;
            cpuC[i][j] = static_cast<double>(random()) / RAND_MAX;
        }

        // add to batches
        dataA.push_back(cpuA[i]);
        dataB.push_back(cpuB[i]);
        dataC.push_back(cpuC[i]);
    }

    ////////////////////////////////////////
    // managed code

    Arrayf64 product_sum;
    { 
        Arrayf64 A = make2(N, N, dataA);
        Arrayf64 B = make2(N, N, dataB);
        Arrayf64 C = make2(N, N, dataC);

        const double alpha = 2;
        const double beta = .5;

        // for each of the PARALLEL, do sum(alpha * A * B + beta * C)
        product_sum = sum( matmul(A, B, C, alpha, beta) );
    }
    const vector< double > result = product_sum.read_scalar(PARALLEL);

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    for (size_t i = 0; i < PARALLEL; i++)
        cout << "result[" << i << "] is " << result[i] << endl;

    exit(0);
}
