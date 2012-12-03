#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <CL/cl.h>
#include <cstdlib>
#include <iostream>

using namespace chai;
using namespace std;

//
// Simple calculation using direct OpenCL with the Chai runtime
//

void exitOnFail(cl_int status, const char* message)
{
    if (CL_SUCCESS != status)
    {
        cout << "error: " << message << endl;
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).noInterpret().initVM();

    ////////////////////////////////////////
    // initialize data buffers on host CPU

    size_t N = 20;
    float cpuX[N], cpuY[N]; // N x 1 row major array buffers

    // initialize array data
    for (size_t i = 0; i < N; i++)
    {
        cpuX[i] = i;
        cpuY[i] = 1;
    }

    float alpha = 1.5f;

    /////////////////////////////////////////
    // create managed arrays

    Arrayf32 X = make1(N, cpuX);
    Arrayf32 Y = make1(N, cpuY);

    ////////////////////////////////////////
    // prepare for OpenCL interoperation

    ForceSchedule();

    ////////////////////////////////////////
    // direct OpenCL not managed by runtime

    // saxpy: Y = alpha * X + Y
    const char *kernelSrc[] = {
        "__kernel void saxpy(const float alpha,",
        "                    __global const float* X,",
        "                    __global float* Y)",
        "{",
        "    Y[get_global_id(0)] += alpha * X[get_global_id(0)];",
        "}" };

    cl_int status;

    // a program can have multiple kernels
    cl_program program = clCreateProgramWithSource(
                             GetContext(), // from VM
                             sizeof(kernelSrc)/sizeof(const char*),
                             kernelSrc,
                             NULL,
                             &status);
    exitOnFail(status, "create program");

    // compile the program
    cl_device_id deviceID = GetDevice(); // from VM
    status = clBuildProgram(program, 1, &deviceID, "", NULL, NULL);
    exitOnFail(status, "build program");

    // one kernel from the program
    cl_kernel kernel = clCreateKernel(program, "saxpy", &status);
    exitOnFail(status, "create kernel");

    // set first argument
    status = clSetKernelArg(kernel, 0, sizeof(float), &alpha);
    exitOnFail(status, "set kernel argument alpha");

    // set second argument
    cl_mem memX = X; // from VM
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &memX);
    exitOnFail(status, "set kernel argument X");

    // set third argument
    cl_mem memY = Y; // from VM
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &memY);
    exitOnFail(status, "set kernel argument Y");

    // N work-items in groups of 4
    const size_t groupsize = 4;
    const size_t global[] = { N }, local[] = { groupsize };

    cl_event event;

    // enqueue kernel
    status = clEnqueueNDRangeKernel(GetCommandQueue(), // from VM
                                    kernel,
                                    sizeof(global)/sizeof(size_t),
                                    NULL,
                                    global,
                                    local,
                                    0,
                                    NULL,
                                    &event);
    exitOnFail(status, "enqueue kernel");

    // wait for kernel, this forces execution
    status = clWaitForEvents(1, &event);
    exitOnFail(status, "wait for enqueue kernel");

    clReleaseEvent(event);
    clReleaseKernel(kernel);
    clReleaseProgram(program);

    ////////////////////////////////////////
    // read back result

    Y.read1(cpuY, N * sizeof(float));

    ////////////////////////////////////////
    // stop Chai runtime

    shutdown();

    ////////////////////////////////////////
    // print result

    for (size_t i = 0; i < N; i++)
        cout << "Y[" << i << "] is " << cpuY[i] << endl;

    exit(0);
}
