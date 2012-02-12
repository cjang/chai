#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 13 of "Public Google PeakStream.ppt")
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
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
            if (oldRRcpu <= TOLERANCE) {
                break;
            }
            x = newX;
        }
    }
    x.read1(cpux, N * sizeof(float));

    return iter;
}

int main(int argc, char *argv[])
{
    /////////////////////////////////////
    // boilerplate: start virtual machine

    ParseArgs pargs(argc, argv);
    if (! pargs.initVM()) // initialize virtual machine
    {
        cerr << "usage: " << argv[0] << " -f configspec" << endl;
        exit(1);
    }

    /////////////////////////////////////
    // computational work

    const int N = 20;
    float cpuA[N * N], cpux[N], cpub[N];

    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = i; j < N; j++)
        {
            float v = random();
            v /= RAND_MAX;
            cpuA[i + j * N] = v;
            cpuA[j + i * N] = v;
        }
    }

    for (size_t i = 0; i < N; i++)
    {
        float v = random();
        v /= RAND_MAX;
        cpub[i] = v;
    }

    cout << "number of iterations is "
         << Conj_Grad_GPU_PS(N, cpuA, cpux, cpub)
         << endl;

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown(); // shutdown virtual machine

    exit(0);
}
