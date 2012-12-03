#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 13 of "Public Google PeakStream.ppt")
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
//
// 1. construct random symmetric positive definite matrix A
// 2. construct vector b
// 3. calculate solution to A * x = b using conjugate gradient algorithm
// 4. read back result for vector x
//

static const float TOLERANCE = 0.1;

int Conj_Grad_GPU_PS(int N, float *cpuA, float *cpux, float *cpub)
{
    int iter;
    Arrayf32 x = Arrayf32::zeros(N);
    {
        Arrayf32 A = Arrayf32::make2(N, N, cpuA);
        Arrayf32 b = Arrayf32::make1(N, cpub);
        Arrayf32 residuals = b - matmul(A, x);
        Arrayf32 p = residuals;
        Arrayf32 newRR = dot_product(residuals, residuals);

        for (iter = 0; iter < N; iter++) {
            Arrayf32 oldRR = newRR;
            Arrayf32 newX, newP, newResiduals;
            Arrayf32 Ap = matmul(A, p);
            Arrayf32 dp = dot_product(p, Ap);
            newX = x + p * oldRR / dp;
            newResiduals = residuals - Ap * oldRR / dp;
            newRR = dot_product(newResiduals, newResiduals);
            newP = newResiduals + p * newRR / oldRR;

            p = newP;
            residuals = newResiduals;

            float oldRRcpu = oldRR.read_scalar();
            cerr << "ITERATION " << iter << " RESIDUAL " << oldRRcpu << endl;
            if (oldRRcpu <= TOLERANCE) {
                break;
            }
            x = newX;
        }
    }
    x.read1(cpux, N * sizeof(float));

    return iter;
}

void randomSymPosDef(const size_t N, float* outMatrix)
{
    // upper triangular matrix for Cholesky factorization
    vector< float > upperTri;

    // random with positive eigenvalues down diagonal
    for (size_t i = 0; i < N; i++)
    {
        // positive eigenvalue, convex enough to converge quickly
        upperTri.push_back( 3 );

        // strictly part of upper triangle
        for (size_t j = i + 1; j < N; j++)
        {
            upperTri.push_back(
                static_cast<float>(random()) / RAND_MAX - .5f );
        }
    }

    // Cholesky product: left is upper triangular, right is lower triangular
    size_t leftStart = 0;
    for (size_t i = 0; i < N; i++)
    {
        size_t rightStart = leftStart;
        for (size_t j = i; j < N; j++)
        {
            double accum = 0;

            for (size_t k = j; k < N; k++)
            {
                accum += upperTri[leftStart + k - i]
                       * upperTri[rightStart + k - j];
            }

            outMatrix[i * N + j] = accum;
            if (i != j)
                outMatrix[j * N + i] = accum;

            rightStart += N - j;
        }

        leftStart += N - i;
    }
}

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).initVM();

    ////////////////////////////////////////
    // initialize data buffers on host CPU

    const int N = 80;
    float cpuA[N * N], cpux[N], cpub[N];

    // random positive symmetric definite matrix A
    randomSymPosDef(N, cpuA);

    // inhomogeneous vector b
    for (size_t i = 0; i < N; i++)
        cpub[i] = 1;

    ////////////////////////////////////////
    // managed code: conjugate gradient

    const int numberIterations = Conj_Grad_GPU_PS(N, cpuA, cpux, cpub);

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    cout << "number of iterations is " << numberIterations << endl;

    exit(0);
}
