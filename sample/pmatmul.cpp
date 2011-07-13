#include <iostream>
#include <map>
#include <peakstream.h>
#include <set>
#include <vector>

#include <omp.h>
#include <pthread.h>

using namespace chai;
using namespace std;

int main(int argc, char *argv[])
{
    const size_t N = 10;

    // buffers on CPU
    double cpuA[N*N], cpuB[N*N];

    const size_t PARA = 10;
    pthread_t ptHand[PARA];

    omp_set_num_threads(600);

    init(); // initialize virtual machine

    #pragma omp parallel for
    for (size_t i = 0; i < PARA; i++)
    {
        ptHand[i] = pthread_self();

        for (size_t j = 0; j < N*N; j++) { cpuA[j] = 1; cpuB[j] = 1; }
        Arrayf64 A[PARA], B[PARA], C[PARA];

        A[i] = make2(N, N, cpuA);
        B[i] = make2(N, N, cpuB);

        C[i] = matmul(A[i], B[i]);

        double cpuC[N*N];

        C[i].read1(cpuC, N*N);
    }

    shutdown(); // shutdown virtual machine

    map< pthread_t, vector< size_t > > ptMap;
    for (size_t i = 0; i < PARA; i++)
        ptMap[ptHand[i]].push_back(i);

    size_t count = 0;
    for (map< pthread_t, vector< size_t > >::const_iterator
         it = ptMap.begin();
         it != ptMap.end();
         it++)
    {
        cout << count++ << "\t";
        for (size_t i = 0; i < (*it).second.size(); i++)
            cout << (*it).second[i] << " ";
        cout << endl;
    }

    return 0;
}
