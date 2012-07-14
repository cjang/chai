#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 36 of "Public Google PeakStream.ppt")
//
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
//

const size_t M = 100;
const size_t N = 100;

float MonteCarloAntithetic(float price,
                           float strike,
                           float vol,
                           float rate,
                           float div,
                           float T)
{
    float deltat        = T / N;
    float muDeltat      = (rate - div - 0.5 * vol * vol) * deltat;
    float volSqrtDeltat = vol * sqrt(deltat);
    float meanCPU       = 0.0f;

    float buf[M];
    for (int i = 0; i < M; i++) buf[i] = 0;

    Arrayf32 meanSP; // result
    {
        RNGf32 rng_hndl(RNG_DEFAULT, 0);
        Arrayf32 U = zeros_f32(M);
        for (int i = 0; i < N; i++)
            U += rng_normal_make(rng_hndl, M);

        Arrayf32 values;
        {
            Arrayf32 lnS1 = log(price) + N * muDeltat + volSqrtDeltat * U;
            Arrayf32 lnS2 = log(price) + N * muDeltat + volSqrtDeltat * (-U);
            Arrayf32 S1 = exp(lnS1);
            Arrayf32 S2 = exp(lnS2);
            values = (0.5 * (max(0, S1 - strike) + max(0, S2 - strike))
                          * exp(-rate * T));
        }
        meanSP = mean(values);
    }
    meanCPU = meanSP.read_scalar();

    return meanCPU; 
}

int main(int argc, char *argv[])
{
    /////////////////////////////////////
    // boilerplate: start virtual machine

    ParseArgs(argc, argv).initVM(); // start virtual machine, exit on error

    /////////////////////////////////////
    // computational work

    float price = 1, strike = 1, vol = 1, rate = 1, div = 1, T = 1;

    cout << "mean is "
         << MonteCarloAntithetic(price, strike, vol, rate, div, T)
         << endl;

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown();

    exit(0);
}
