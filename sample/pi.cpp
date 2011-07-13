#include <iostream>
#include <stdio.h>
#include <peakstream.h>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 17 of "Public Google PeakStream.ppt")
//
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
//

#define NSET 1000000         // number of monte carlo trials

Arrayf32
compute_pi(void)
{
    RNGf32 G(SP_RNG_DEFAULT); // create an RNG
    Arrayf32 X = rng_uniform_make(G, NSET, 1, 0.0, 1.0);
    Arrayf32 Y = rng_uniform_make(G, NSET, 1, 0.0, 1.0);
    Arrayf32 distance_from_zero = sqrt(X * X + Y * Y);
    Arrayf32 inside_circle = (distance_from_zero <= 1.0f);
    return 4.0f * sum(inside_circle) / NSET;
}

int main(int argc, char *argv[])
{
    init();

    { // force array destructors before shutdown

    Arrayf32 Pi = compute_pi();  // get the answer as a 1x1 array
    float pi = Pi.read_scalar(); // convert answer to a simple float
    printf("Value of Pi = %f\n", pi);

    } // force array destructors before shutdown

    shutdown();

    return 0;
}
