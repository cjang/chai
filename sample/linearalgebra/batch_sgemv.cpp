#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace chai;
using namespace std;

//
// Parallel general matrix vector multiplication and reduction of products
// in single precision
//
// 1. create PARALLEL managed arrays for A, B, C
// 2. calculate C = alpha * A * B + beta * C in parallel (note: matrix vector)
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
    float cpuA[PARALLEL][N * N];
    float cpuB[PARALLEL][N];
    float cpuC[PARALLEL][N];

    // batched arrays
    vector< float* > dataA, dataB, dataC;

    // initialize arrays
    for (size_t i = 0; i < PARALLEL; i++)
    {
        // fill matrix A with random numbers
        for (size_t j = 0; j < N * N; j++)
        {
            cpuA[i][j] = static_cast<float>(random()) / RAND_MAX;
        }

        // fill vectors B and C with random numbers
        for (size_t j = 0; j < N; j++)
        {
            cpuB[i][j] = static_cast<float>(random()) / RAND_MAX;
            cpuC[i][j] = static_cast<float>(random()) / RAND_MAX;
        }

        // add to batches
        dataA.push_back(cpuA[i]);
        dataB.push_back(cpuB[i]);
        dataC.push_back(cpuC[i]);
    }

    ////////////////////////////////////////
    // managed code

    Arrayf32 product_sum;
    { 
        Arrayf32 A = make2(N, N, dataA);
        Arrayf32 B = make1(N, dataB);
        Arrayf32 C = make1(N, dataC);

        const float alpha = 2;
        const float beta = .5;

        // for each of the PARALLEL, do sum(alpha * A * B + beta * C)
        product_sum = sum( matmul(A, B, C, alpha, beta) );
    }
    const vector< float > result = product_sum.read_scalar(PARALLEL);

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    for (size_t i = 0; i < PARALLEL; i++)
        cout << "result[" << i << "] is " << result[i] << endl;

    exit(0);
}
