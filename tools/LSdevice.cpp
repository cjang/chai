// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <omp.h>
#include <set>

#include "OCLinit.hpp"

using namespace chai_internal;
using namespace std;

int main(int argc, char *argv[])
{
    OCLinit ocl;

    const set< size_t > cpuidx = ocl.cpuIndexes();
    const set< size_t > gpuidx = ocl.gpuIndexes();
    const set< size_t > accidx = ocl.accIndexes();

    for (size_t i = 0; i < ocl.numberDevices(); i++)
    {
        cout << i << " | ";

        if (cpuidx.count(i)) cout << "CPU";
        if (gpuidx.count(i)) cout << "GPU";
        if (accidx.count(i)) cout << "ACC";

        cout << " | " << ocl.devices().name(i)
             << " | " << ocl.devices().vendor(i)
             << endl;
    }

    return 0;
}
