#include <iostream>
#include <math.h>
#include <peakstream.h>

using namespace chai;
using namespace std;

//
// Sample code from a presentation given by Matthew Papakipos at Stanford in
// 2007 (page 35 of "Public Google PeakStream.ppt")
//
// http://www.stanford.edu/class/ee380/Abstracts/070926-PeakStream.pdf
//

const float LX = 100;
const float LT = 100;
const float velhalf = 1;

void
KirchhoffMigration(int NT, int N, float *datagpu, float *modlgpu)
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

// modified code the JIT can loop roll
        Arrayf32 y = 0;
        Arrayf32 index1 = zeros_f32(NT, N);
        Arrayf32 it;
        for (int iy = 0; iy < N; iy++)
        {
            y += dx;
            index1 += ones_f32(NT, N);
            it = 0.5 + sqrt(z * z + (x - y) * (x - y) * factor) * idt;
            modl += gather2_floor(data, it, index1);
        }

/* original code the JIT is unable to loop roll
        for (int iy = 0; iy < N; iy++)
        {
            float y = float(iy) * dx;
            Arrayf32 index1 = float(iy) * ones_f32(NT, N);
            Arrayf32 it = 0.5 + sqrt(z * z + (x - y) * (x - y) * factor) * idt;
            modl += gather2_floor(data, it, index1);
        }
*/
    }
    modl.read1(modlgpu, NTN * sizeof(float));

    return;
}

int main(int argc, char *argv[])
{
    init();

    int NT = 100, N = 100;
    float datagpu[NT * N], modlgpu[NT * N];

    KirchhoffMigration(NT, N, datagpu, modlgpu);

    shutdown();

    return 0;
}
