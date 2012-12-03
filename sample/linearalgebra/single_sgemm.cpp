#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace chai;
using namespace std;

//
// Single precision matrix multiply and reduction of matrix product
//
// 1. create managed arrays for A, B, C
// 2. calculate C = A * B
// 3. calculate sum(C)
// 4. read back result
//

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).initVM();

    ////////////////////////////////////////
    // initialize data buffers on host CPU

    const size_t N = 80;

    // array buffers
    float cpuA[N * N];
    float cpuB[N * N];
    float cpuC[N * N];

    // fill arrays with random numbers
    for (size_t i = 0; i < N * N; i++)
    {
        cpuA[i] = static_cast<float>(random()) / RAND_MAX - .5f;
        cpuB[i] = static_cast<float>(random()) / RAND_MAX - .5f;
        cpuC[i] = static_cast<float>(random()) / RAND_MAX - .5f;
    }

    ////////////////////////////////////////
    // managed code

    Arrayf32 product_sum;
    { 
        Arrayf32 A = make2(N, N, cpuA);
        Arrayf32 B = make2(N, N, cpuB);
        Arrayf32 C = make2(N, N, cpuC);

        // sum(A * B)
        product_sum = sum( matmul(A, B) );
    }
    const float result = product_sum.read_scalar();

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    cout << "result is " << result << endl;

    exit(0);
}
