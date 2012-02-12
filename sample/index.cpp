#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>

using namespace chai;
using namespace std;

void body(void)
{
    const float dx = .25;

    // managed code
    Arrayf32 C;
    {
        Arrayf32 A = index_f32(0, 10, 10);
        Arrayf32 B = dx * index_f32(1, 10, 10);

        C = sum(exp(A) + transpose(B));
    }
    const float c = C.read_scalar();

    cout << "result is " << c << endl;
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

    body();

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown(); // shutdown virtual machine

    exit(0);
}
