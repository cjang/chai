#include <iostream>
#include <math.h>
#include <peakstream.h>

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
    Arrayf32 meanSP; // result
    {                // a new scope to hold temporary arrays
        RNGf32 rng_hndl(SP_RNG_DEFAULT);
        Arrayf32 U = zeros_f32(M);
        for (int i = 0; i < N; i++) {
            U += rng_normal_make(rng_hndl, M);
        }
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
    }                // all temporaries released as we exit scope
    meanCPU = meanSP.read_scalar();

    return meanCPU; 
}

int main(int argc, char *argv[])
{
    init();

    float price = 1, strike = 1, vol = 1, rate = 1, div = 1, T = 1;
    cout << "mean is "
         << MonteCarloAntithetic(price, strike, vol, rate, div, T)
         << endl;

    shutdown();

    return 0;
}