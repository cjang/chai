#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 36 of "Public Google PeakStream.ppt")
//
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
//

const size_t M = 100000;
const size_t N = 100;

vector< double > MonteCarloAntithetic(float price,
                                      float strike,
                                      vector< float >& volVec,
                                      vector< double >& rateVec,
                                      float div,
                                      float T)
{
    float deltat = T / N;

    Arrayf64 meanSP; // result
    {
        Arrayf32 vol           = make1(volVec);
        Arrayf64 rate          = make1(rateVec);
        Arrayf32 muDeltat      = (rate - div - 0.5 * vol * vol) * deltat;
        Arrayf32 volSqrtDeltat = vol * sqrt(deltat);

        RNGf64 rng_hndl(RNG_DEFAULT, 0);

        Arrayf64 U = zeros_f64(M);
        for (int i = 0; i < N; i++)
            U += rng_normal_make(rng_hndl, M);

        Arrayf64 values;
        {
            Arrayf64 lnS1 = log(price)
                            + double(N) * muDeltat
                            + volSqrtDeltat * U;

            Arrayf64 lnS2 = log(price)
                            + double(N) * muDeltat
                            + volSqrtDeltat * (-U);

            Arrayf64 S1 = exp(lnS1);
            Arrayf64 S2 = exp(lnS2);

            values = (0.5 * (max(0, S1 - strike) + max(0, S2 - strike))
                          * exp(-rate * T));
        }
        meanSP = mean(values);
    }

    const size_t numExperiments = volVec.size() > rateVec.size()
                                      ? volVec.size()
                                      : rateVec.size();

    return meanSP.read_scalar(numExperiments);
}

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).initVM();

    ////////////////////////////////////////
    // option parameters

    const float price = 60;
    const float strike = 65;

    // combinations of (volatility, rate)
    vector< float > volatility;
    vector< double > rate;
    for (size_t i = 0; i < 10; i++)
    {
        const float vol = .3 + i * .01;
        const double r = .01 - i * .001;

        volatility.push_back( vol );
        rate.push_back( r );
    }

    const float dividend = 0;
    const float T = 0.25;

    ////////////////////////////////////////
    // managed code: estimate prices

    const vector< double > optionPrice
        = MonteCarloAntithetic(price, strike, volatility, rate, dividend, T);

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print results

    cout << "European call option for:" << endl
         << endl
         << "  price              = $" << price << endl
         << "  strike             = $" << strike << endl
         << "  dividend payment   = " << (dividend * 100.f) << "%" << endl
         << "  time to maturity   = " << T << " years" << endl
         << endl
         << "and (volatility, rate) are:" << endl
         << endl;

    for (size_t i = 0; i < optionPrice.size(); i++)
        cout << "  ("
             << (volatility[i] * 100.f)
             << "%, "
             << (rate[i] * 100.f)
             << "%) is $"
             << optionPrice[i]
             << endl;

    exit(0);
}
