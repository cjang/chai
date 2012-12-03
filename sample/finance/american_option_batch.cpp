#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace chai;
using namespace std;

//
// American style call option using recombining binomial tree
//

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).noInterpret().initVM();

    ////////////////////////////////////////
    // option parameters

    const double price = 100;
    const double rate = .10;
    const double maturity = 1.;

    const size_t steps = 100;

    const int NUM       = 20;
    const int GROUPSIZE = 5;

    // a volatility smile
    vector< double > strike, volatility;
    for (int i = 0; i < NUM; i++)
    {
        const double X = 100. - NUM/2 + i;
        strike.push_back(X);

        const double sigma = .25 + .1 * 4 * (i-NUM/2) * (i-NUM/2) / (NUM*NUM);
        volatility.push_back(sigma);
    }

    ////////////////////////////////////////
    // managed code: binomial tree option

    stringstream ss;

    ss << "#pragma OPENCL EXTENSION cl_khr_fp64" << endl
       << "__kernel void americanBatch(__global double * optionValues, "
                                      "__global const double * strike, "
                                      "__global const double * volatility, "
                                      "const double price, "
                                      "const double rate, "
                                      "const double maturity) "
          "{"
          "  __local double prices[" << (steps+1) * GROUPSIZE << "];"
          "  __local double option[" << (steps+1) * GROUPSIZE << "];"
          ""
          "  const uint ROW = " << (steps+1) << " * get_local_id(0);"
          "  const double vol = volatility[get_global_id(0)];"
          "  const double X = strike[get_global_id(0)];"
          "  const double R = exp(rate * (maturity/" << steps << "));"
          "  const double up = exp(vol * sqrt(maturity/" << steps << "));"
          "  const double down = 1. / up;"
          "  const double prUp = (R - down) / (up - down);"
          ""
          "  double tmp = price * pown(down, " << steps << ");"
          ""
          "  prices[0 + ROW] = tmp;"
          "  option[0 + ROW] = max(tmp - X, 0.);"
          "  for (int i = 1; i < " << (steps + 1) << "; i++)"
          "  {"
          "    tmp = tmp * up * up;"
          "    prices[i + ROW] = tmp;"
          "    option[i + ROW] = max(tmp - X, 0.);"
          "  }"
          ""
          "  for (int i = " << steps << "; i > 0; i--)"
          "  {"
          "    for (int j = 0; j < i; j++)"
          "    {"
          "        tmp = (prUp * option[j + 1 + ROW]"
                        " + (1. - prUp) * option[j + ROW]) / R;"
          "        prices[j + ROW] = down * prices[j + 1 + ROW];"
          "        option[j + ROW] = max(prices[j + ROW] - X, tmp);"
          "    }"
          "  }"
          ""
          "  optionValues[get_global_id(0)] = option[ROW];"
          "}";

    ProgramCL inlineCL( ss.str() );

    double optionPrice[NUM];
    Arrayf64 option = make1(NUM, optionPrice);
    Arrayf64 X = make1(NUM, &strike[0]);
    Arrayf64 sigma = make1(NUM, &volatility[0]);

    (inlineCL, "americanBatch",
               option,
               X,
               sigma,
               price,
               rate,
               maturity)(NUM, GROUPSIZE);

    option.read1(optionPrice, NUM * sizeof(double));

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print results

    cout << "American call option for:" << endl
         << endl
         << "  price              = $" << price << endl
         << "  risk-free rate     = " << (rate * 100.) << "%" << endl
         << "  time to maturity   = " << maturity << " years" << endl
         << endl
         << "and (strike, volatility) are:" << endl
         << endl;

    for (size_t i = 0; i < NUM; i++)
        cout << "  ($"
             << strike[i]
             << ", "
             << (volatility[i] * 100.)
             << "%) is $"
             << optionPrice[i]
             << endl;

    exit(0);
}
