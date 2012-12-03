#include <CL/cl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

//
// Simple calculation entirely in OpenCL, maximum amount of boilerplate
//

void exitOnFail(cl_int status, const char* message)
{
    if (CL_SUCCESS != status)
    {
        printf("error: %s\n", message);
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    // return code used by OpenCL API
    cl_int status;

    // wait event synchronization handle used by OpenCL API
    cl_event event;

    ////////////////////////////////////////
    // discover platforms

    // determine number of platforms
    cl_uint numPlatforms;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    exitOnFail(status, "number of platforms");

    // get platform IDs
    cl_platform_id platformIDs[numPlatforms];
    status = clGetPlatformIDs(numPlatforms, platformIDs, NULL);
    exitOnFail(status, "get platform IDs");

    ////////////////////////////////////////
    // discover compute devices

    // look for a CPU and GPU compute device
    cl_platform_id cpuPlatformID, gpuPlatformID;
    cl_device_id cpuDeviceID, gpuDeviceID;
    int isCPU = 0, isGPU = 0;

    // iterate over platforms
    for (size_t i = 0; i < numPlatforms; i++)
    {
        // determine number of devices for a platform
        cl_uint numDevices;
        status = clGetDeviceIDs(platformIDs[i],
                                CL_DEVICE_TYPE_ALL,
                                0,
                                NULL,
                                &numDevices);
        if (CL_SUCCESS == status)
        {
            // get device IDs for a platform
            cl_device_id deviceIDs[numDevices];
            status = clGetDeviceIDs(platformIDs[i],
                                    CL_DEVICE_TYPE_ALL,
                                    numDevices,
                                    deviceIDs,
                                    NULL);
            if (CL_SUCCESS == status)
            {
                // iterate over devices
                for (size_t j = 0; j < numDevices; j++)
                {
                    cl_device_type deviceType;
                    status = clGetDeviceInfo(deviceIDs[j],
                                             CL_DEVICE_TYPE,
                                             sizeof(cl_device_type),
                                             &deviceType,
                                             NULL);
                    if (CL_SUCCESS == status)
                    {
                        // first CPU device
                        if (!isCPU && (CL_DEVICE_TYPE_CPU & deviceType))
                        {
                            isCPU = 1;
                            cpuPlatformID = platformIDs[i];
                            cpuDeviceID = deviceIDs[j];
                        }

                        // first GPU device
                        if (!isGPU && (CL_DEVICE_TYPE_GPU & deviceType))
                        {
                            isGPU = 1;
                            gpuPlatformID = platformIDs[i];
                            gpuDeviceID = deviceIDs[j];
                        }
                    }
                }
            }
        }
    }

    // pick GPU device if it exists, otherwise use CPU
    cl_platform_id platformID;
    cl_device_id deviceID;
    if (isGPU)
    {
        platformID = gpuPlatformID;
        deviceID = gpuDeviceID;
    }
    else if (isCPU)
    {
        platformID = cpuPlatformID;
        deviceID = cpuDeviceID;
    }
    else
    {
        // no devices found
        exitOnFail(CL_DEVICE_NOT_FOUND, "no devices found");
    }

    ////////////////////////////////////////
    // create context

    cl_context_properties props[] = { CL_CONTEXT_PLATFORM,
                                      (cl_context_properties) platformID,
                                      0 };
    cl_context context = clCreateContext(props,
                                         1,
                                         &deviceID,
                                         NULL,
                                         NULL,
                                         &status);
    exitOnFail(status, "create context");

    ////////////////////////////////////////
    // create command queue

    cl_command_queue queue = clCreateCommandQueue(context,
                                                  deviceID,
                                                  0,
                                                  &status);
    exitOnFail(status, "create command queue");

    ////////////////////////////////////////
    // create memory buffers

    size_t N = 20;
    float cpuX[N], cpuY[N]; // N x 1 row major array buffers

    // initialize array data
    for (size_t i = 0; i < N; i++)
    {
        cpuX[i] = i;
        cpuY[i] = 1;
    }

    // second argument: memory buffer object for X
    cl_mem memX = clCreateBuffer(context,
                                 CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                 N * sizeof(float),
                                 cpuX,
                                 &status);
    exitOnFail(status, "create buffer for X");

    // third argument: memory buffer object for Y
    cl_mem memY = clCreateBuffer(context,
                                 CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                 N * sizeof(float),
                                 cpuY,
                                 &status);
    exitOnFail(status, "create buffer for Y");

    ////////////////////////////////////////
    // move buffer data to device

    // data transfer for array X
    status = clEnqueueWriteBuffer(queue,
                                  memX,
                                  CL_FALSE,
                                  0,
                                  N * sizeof(float),
                                  cpuX,
                                  0,
                                  NULL,
                                  &event);
    exitOnFail(status, "write X to device");
    status = clWaitForEvents(1, &event);
    exitOnFail(status, "wait for write X to device");
    clReleaseEvent(event);

    // data transfer for array Y
    status = clEnqueueWriteBuffer(queue,
                                  memY,
                                  CL_FALSE,
                                  0,
                                  N * sizeof(float),
                                  cpuY,
                                  0,
                                  NULL,
                                  &event);
    exitOnFail(status, "write Y to device");
    status = clWaitForEvents(1, &event);
    exitOnFail(status, "wait for write Y to device");
    clReleaseEvent(event);

    ////////////////////////////////////////
    // create program and kernel

    // saxpy: Y = alpha * X + Y
    const char *kernelSrc[] = {
        "__kernel void saxpy(const float alpha,",
        "                    __global const float* X,",
        "                    __global float* Y)",
        "{",
        "    Y[get_global_id(0)] += alpha * X[get_global_id(0)];",
        "}" };

    // a program can have multiple kernels
    cl_program program = clCreateProgramWithSource(
                             context,
                             sizeof(kernelSrc)/sizeof(const char*),
                             kernelSrc,
                             NULL,
                             &status);
    exitOnFail(status, "create program");

    // compile the program
    status = clBuildProgram(program, 1, &deviceID, "", NULL, NULL);
    exitOnFail(status, "build program");

    // one kernel from the program
    cl_kernel kernel = clCreateKernel(program, "saxpy", &status);
    exitOnFail(status, "create kernel");

    ////////////////////////////////////////
    // set kernel arguments

    // first argument: a scalar float
    float alpha = 1.5f;

    // set first argument
    status = clSetKernelArg(kernel, 0, sizeof(float), &alpha);
    exitOnFail(status, "set kernel argument alpha");

    // set second argument
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &memX);
    exitOnFail(status, "set kernel argument X");

    // set third argument
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &memY);
    exitOnFail(status, "set kernel argument Y");

    ////////////////////////////////////////
    // enqueue kernel and wait for it

    // N work-items in groups of 4
    const size_t groupsize = 4;
    const size_t global[] = { N }, local[] = { groupsize };

    // enqueue kernel
    status = clEnqueueNDRangeKernel(queue,
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

    ////////////////////////////////////////
    // read back buffer data from device

    // data transfer for array Y
    status = clEnqueueReadBuffer(queue,
                                 memY,
                                 CL_FALSE,
                                 0,
                                 N * sizeof(float),
                                 cpuY,
                                 0,
                                 NULL,
                                 &event);
    exitOnFail(status, "read Y from device");
    status = clWaitForEvents(1, &event);
    exitOnFail(status, "wait for read Y from device");
    clReleaseEvent(event);

    ////////////////////////////////////////
    // cleanup runtime objects

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(memX);
    clReleaseMemObject(memY);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    ////////////////////////////////////////
    // print result

    for (size_t i = 0; i < N; i++)
        printf("Y[%u] is %f\n", (int)i, (double)cpuY[i]);

    exit(0);
}
