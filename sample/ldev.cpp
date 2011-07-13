#include <iostream>
#include <omp.h>

#include "OCLinit.hpp"

using namespace chai_internal;
using namespace std;

int main(int argc, char *argv[])
{
    OCLinit ocl;

    for (size_t i = 0; i < ocl.numberDevices(); i++)
    {
        cout << i << endl;
        cout << "\t" << ocl.devices().name(i) << endl;
        cout << "\t" << ocl.devices().vendor(i) << endl;
        cout << endl;
    }

    cout << "omp_get_max_threads() " << omp_get_max_threads() << endl;
    cout << "omp_get_num_procs() " << omp_get_num_procs() << endl;

    return 0;
}
