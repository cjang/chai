#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 36 of "Public Google PeakStream.ppt")
//
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
//

const size_t M = 10000;
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
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).initVM();

    ////////////////////////////////////////
    // option parameters

    // From Espen Haug's Excel sheet example:
    // http://www.espenhaug.com/black_scholes.html
    const float price = 60;
    const float strike = 65;
    const float volatility = .3;
    const float rate = .08;
    const float dividend = 0;
    const float T = 0.25;

    ////////////////////////////////////////
    // managed code: estimate price

    const float optionPrice
        = MonteCarloAntithetic(price, strike, volatility, rate, dividend, T);

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    cout << "European call option for:" << endl
         << endl
         << "  price              = $" << price << endl
         << "  strike             = $" << strike << endl
         << "  volatility         = " << (volatility * 100.f) << "%" << endl
         << "  risk-free interest = " << (rate * 100.f) << "%" << endl
         << "  dividend payment   = " << (dividend * 100.f) << "%" << endl
         << "  time to maturity   = " << T << " years" << endl
         << endl
         << "is $"
         << optionPrice
         << endl;

    exit(0);
}
