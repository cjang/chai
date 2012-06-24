#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>

using namespace chai;
using namespace std;

void filter(float* cpuA, float* cpuB, const size_t N)
{
    Arrayf32 B;
    {
        Arrayf32 X = index_f32(0, N, N);
        Arrayf32 Y = index_f32(1, N, N);

        Arrayf32 A = make2(N, N, cpuA);

        B = zeros_f32(N, N);
        B += gather2_floor(A, X, Y);
        B += gather2_floor(A, X - 1, Y);
        B += gather2_floor(A, X + 1, Y);
        B += gather2_floor(A, X, Y - 1);
        B += gather2_floor(A, X, Y + 1);
    }
    B.read1(cpuB, N * N * sizeof(float));
}

int main(int argc, char *argv[])
{
    /////////////////////////////////////
    // boilerplate: start virtual machine

    ParseArgs(argc, argv).initVM(); // start virtual machine, exit on error

    /////////////////////////////////////
    // computational work

    const size_t N = 12;
    float cpuA[N * N], cpuB[N * N];
    for (size_t i = 0; i < N; i++)
    for (size_t j = 0; j < N; j++)
    {
        cpuA[i * N + j] = i + j;
        cpuB[i * N + j] = 0;
    }

    filter(cpuA, cpuB, N);

    for (size_t i = 0; i < N; i++)
    {
        cout << "[" << i << "]";

        for (size_t j = 0; j < N; j++)
        {
            cout << " " << cpuB[i * N + j];
        }

        cout << endl;
    }

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown(); // shutdown virtual machine

    exit(0);
}
