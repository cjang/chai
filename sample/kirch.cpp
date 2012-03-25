#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <iostream>
#include <stdlib.h>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 35 of "Public Google PeakStream.ppt")
//
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
//

static int NNN = 10;

//const float LX = 100;
//const float LT = 100;
const float LX = NNN;
const float LT = NNN;
const float velhalf = 1;

void KirchhoffMigration(int NT, int N, float *datagpu, float *modlgpu)
{
    int NTN = NT * N;
    float dx = LX / float(N);
    float dt = LT / float(NT);
    float factor = 1./ (velhalf * velhalf);
    float idt = 1./ dt;
    Arrayf32 modl = zeros_f32(NT, N);
    {
        Arrayf32 x = dx * index_f32(1, NT, N);
        Arrayf32 z = dt * index_f32(0, NT, N);
        Arrayf32 data = Arrayf32::make2(NT, N, datagpu);

        for (int iy = 0; iy < N; iy++)
        {
            float y = float(iy) * dx;
            Arrayf32 index1 = float(iy) * ones_f32(NT, N);
            Arrayf32 it = 0.5 + sqrt(z * z + (x - y) * (x - y) * factor) * idt;
            modl += gather2_floor(data, it, index1);
        }
    }
    modl.read1(modlgpu, NTN * sizeof(float));

    return;
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

//    int NT = 100, N = 100;
    int NT = NNN, N = NNN;
    float datagpu[NT * N], modlgpu[NT * N];
    for (size_t i = 0; i < N; i++)
    for (size_t j = 0; j < NT; j++)
        datagpu[i * NT + j] = 1. / (1 + i * j);

    KirchhoffMigration(NT, N, datagpu, modlgpu);

    for (size_t i = 0; i < N; i++)
    {
        cout << "[" << i << "]";

        for (size_t j = 0; j < NT; j++)
            cout << " " << modlgpu[i * NT + j];

        cout << endl;
    }

    /////////////////////////////////////
    // boilerplate: stop virtual machine

    shutdown(); // shutdown virtual machine

    exit(0);
}
