#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace chai;
using namespace std;

//
// American style call option using recombining binomial tree
//

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).initVM();

    ////////////////////////////////////////
    // option parameters

    const double price = 100;
    const double strike = 100;
    const double volatility = .25;
    const double rate = .10;
    const double maturity = 1.;

    const size_t steps  = 100;
    const double R      = exp(rate * (maturity / steps));
    const double Rinv   = 1. / R;
    const double up     = exp(volatility * sqrt(maturity / steps));
    const double down   = 1. / up;
    const double prUp   = (R - down) / (up - down);
    const double prDown = 1. - prUp;

    ////////////////////////////////////////
    // underlying prices

    double pricesCPU[ steps + 1 ];

    pricesCPU[0] = price * pow(down, steps);
    for (size_t i = 1; i <= steps; i++)
    {
        pricesCPU[i] = pricesCPU[i-1] * up * up;
    }

    ////////////////////////////////////////
    // option values

    double optionCPU[ steps + 1 ];

    for (size_t i = 0; i < steps + 1; i++)
    {
        const double payoff = pricesCPU[i] - strike;
        optionCPU[ i ] = payoff > 0 ? payoff : 0;
    }

    ////////////////////////////////////////
    // managed code: binomial tree option

    const size_t numsteps = steps;

    for (size_t i = 0; i < numsteps; i++)
    {
        const size_t N = steps + 1 - i;

        Arrayf64 prices = make1(N, pricesCPU);
        Arrayf64 option = make1(N, optionCPU);

        Arrayf64 IDX = index_f64(0, N);

        Arrayf64 no_exercise
            = (prDown * gather1_floor(option, IDX) +
               prUp * gather1_floor(option, IDX + 1))
                   / exp(rate * (maturity / steps));

        prices = up * prices;
        option = max(no_exercise, prices - strike);

        prices.read1(pricesCPU, N * sizeof(double));
        option.read1(optionCPU, N * sizeof(double));
    }

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    for (size_t i = 0; i < steps + 1 - numsteps; i++)
    {
        cout << "[" << i << "] " << optionCPU[i]
             << "\tprice " << pricesCPU[i]
             << endl;
    }

    exit(0);
}
