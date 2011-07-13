#include <peakstream.h>
#include <pthread.h>

using namespace chai;
using namespace std;

double cpuA[100], cpuB[100];

void* calc(void* ctx)
{
    for (size_t i = 0; i < 10; i++)
    {

    Arrayf64 A = make1(100, cpuA);
    Arrayf64 B = make1(100, cpuB);
    Arrayf64 C = sum(A + B);
    double c = C.read_scalar();

    }

    return NULL;
}

int main(int argc, char *argv[])
{
    init(); // initialize virtual machine

    const size_t PARA = 256;

    pthread_t hand[PARA];

    for (size_t i = 0; i < PARA; i++)
    {
        pthread_create(&hand[i], NULL, calc, NULL);
    }

    for (size_t i = 0; i < PARA; i++)
    {
        void* status;
        pthread_join(hand[i], &status);
    }

    shutdown(); // shutdown virtual machine

    return 0;
}
