// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "OCLinit.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// platforms

OCLplatforms::OCLplatforms(void)
{
    cl_uint numberPlatforms;
    if (CL_SUCCESS != clGetPlatformIDs(0, NULL, &numberPlatforms))
        return;

    cl_platform_id platformIDs[numberPlatforms];
    if (CL_SUCCESS != clGetPlatformIDs(numberPlatforms, platformIDs, NULL))
        return;

    for (size_t i = 0; i < numberPlatforms; i++)
        _handles.push_back(platformIDs[i]);
}

size_t OCLplatforms::size(void) const
{
    return _handles.size();
}

cl_platform_id& OCLplatforms::operator[] (const size_t i)
{
    return _handles[i];
}

const cl_platform_id& OCLplatforms::operator[] (const size_t i) const
{
    return _handles[i];
}

////////////////////////////////////////
// devices

void OCLdevices::insertDevice(const cl_device_id deviceID,
                              const size_t platformIndex)
{
    bool found = false;
    for (size_t i = 0; i < _handles.size(); i++)
        if (deviceID == _handles[i])
            found = true;

    if (found) return;

    _handles.push_back(deviceID);
    _platform.push_back(platformIndex);

    char buffer[512];

    _name.push_back(
        CL_SUCCESS == clGetDeviceInfo(deviceID,
                                      CL_DEVICE_NAME,
                                      sizeof(buffer),
                                      buffer,
                                      NULL)
        ? buffer
        : "");

    _vendor.push_back(
        CL_SUCCESS == clGetDeviceInfo(deviceID,
                                      CL_DEVICE_VENDOR,
                                      sizeof(buffer),
                                      buffer,
                                      NULL)
        ? buffer
        : "");

    size_t value;

    _maxWorkGroupSize.push_back(
        CL_SUCCESS == clGetDeviceInfo(deviceID,
                                      CL_DEVICE_MAX_WORK_GROUP_SIZE,
                                      sizeof(size_t),
                                      &value,
                                      NULL)
        ? value
        : 0);

    cl_device_type devtype;

    if (CL_SUCCESS == clGetDeviceInfo(deviceID,
                                      CL_DEVICE_TYPE,
                                      sizeof(cl_device_type),
                                      &devtype,
                                      NULL))
    {
        _isCPU.push_back(CL_DEVICE_TYPE_CPU & devtype);
        _isGPU.push_back(CL_DEVICE_TYPE_GPU & devtype);
        _isACC.push_back(CL_DEVICE_TYPE_ACCELERATOR & devtype);
    }
    else
    {
        _isCPU.push_back(false);
        _isGPU.push_back(false);
        _isACC.push_back(false);
    }
}

OCLdevices::OCLdevices(const OCLplatforms& platforms)
{
    for (size_t pi = 0; pi < platforms.size(); pi++)
    {
        cl_uint numberDevices;
        if (CL_SUCCESS != clGetDeviceIDs(platforms[pi],
                                         CL_DEVICE_TYPE_ALL,
                                         0,
                                         NULL,
                                         &numberDevices))
            continue; // ignore

        cl_device_id deviceIDs[numberDevices];
        if (CL_SUCCESS != clGetDeviceIDs(platforms[pi],
                                         CL_DEVICE_TYPE_ALL,
                                         numberDevices,
                                         deviceIDs,
                                         NULL))
            continue; // ignore

        for (size_t di = 0; di < numberDevices; di++)
            insertDevice(deviceIDs[di], pi);
    }
}

size_t OCLdevices::size(void) const
{
    return _handles.size();
}

cl_device_id& OCLdevices::operator[] (const size_t i)
{
    return _handles[i];
}

const cl_device_id& OCLdevices::operator[] (const size_t i) const
{
    return _handles[i];
}

size_t OCLdevices::platform(const size_t i) const
{
    return _platform[i];
}

string OCLdevices::name(const size_t i) const
{
    return _name[i];
}

string OCLdevices::vendor(const size_t i) const
{
    return _vendor[i];
}

size_t OCLdevices::maxWorkGroupSize(const size_t i) const
{
    return _maxWorkGroupSize[i];
}

bool OCLdevices::isCPU(const size_t i) const
{
    return _isCPU[i];
}

bool OCLdevices::isGPU(const size_t i) const
{
    return _isGPU[i];
}

bool OCLdevices::isACC(const size_t i) const
{
    return _isACC[i];
}

////////////////////////////////////////
// contexts

OCLcontexts::OCLcontexts(const OCLplatforms& platforms,
                         const OCLdevices& devices)
{
    for (size_t i = 0; i < devices.size(); i++)
    {
        cl_context_properties props[] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties) platforms[devices.platform(i)],
            0 };

        cl_int status;
        const cl_context context = clCreateContext(props,
                                                   1,
                                                   &devices[i],
                                                   NULL,
                                                   NULL,
                                                   &status);

        _handles.push_back(CL_SUCCESS == status ? context : 0);
    }
}

OCLcontexts::~OCLcontexts(void)
{
    for (size_t i = 0; i < _handles.size(); i++)
        if (_handles[i])
            clReleaseContext(_handles[i]);
}

size_t OCLcontexts::size(void) const
{
    return _handles.size();
}

cl_context& OCLcontexts::operator[] (const size_t i)
{
    return _handles[i];
}

const cl_context& OCLcontexts::operator[] (const size_t i) const
{
    return _handles[i];
}

////////////////////////////////////////
// queues

OCLqueues::OCLqueues(const OCLdevices& devices,
                     const OCLcontexts& contexts)
{
    for (size_t i = 0; i < devices.size(); i++)
    {
        cl_int status;
        const cl_command_queue queue
            = clCreateCommandQueue(contexts[i],
                                   devices[i],
                                   CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE |
                                   CL_QUEUE_PROFILING_ENABLE,
                                   &status);

        _handles.push_back(CL_SUCCESS == status ? queue : 0);
    }
}

OCLqueues::~OCLqueues(void)
{
    for (size_t i = 0; i < _handles.size(); i++)
        if (_handles[i])
            clReleaseCommandQueue(_handles[i]);
}

size_t OCLqueues::size(void) const
{
    return _handles.size();
}

cl_command_queue& OCLqueues::operator[] (const size_t i)
{
    return _handles[i];
}

const cl_command_queue& OCLqueues::operator[] (const size_t i) const
{
    return _handles[i];
}

////////////////////////////////////////
// init

OCLinit::OCLinit(void)
    : _platforms(),
      _devices(_platforms),
      _contexts(_platforms, _devices),
      _queues(_devices, _contexts)
{ }

size_t OCLinit::numberDevices(void) const { return _devices.size(); }

OCLplatforms& OCLinit::platforms(void) { return _platforms; }
const OCLplatforms& OCLinit::platforms(void) const { return _platforms; }

OCLdevices& OCLinit::devices(void) { return _devices; }
const OCLdevices& OCLinit::devices(void) const { return _devices; }

OCLcontexts& OCLinit::contexts(void) { return _contexts; }
const OCLcontexts& OCLinit::contexts(void) const { return _contexts; }

OCLqueues& OCLinit::queues(void) { return _queues; }
const OCLqueues& OCLinit::queues(void) const { return _queues; }

set< size_t > OCLinit::cpuIndexes(void) const
{
    set< size_t > s;

    for (size_t i = 0; i < _devices.size(); i++)
        if (_devices.isCPU(i))
            s.insert(i);

    return s;
}

set< size_t > OCLinit::gpuIndexes(void) const
{
    set< size_t > s;

    for (size_t i = 0; i < _devices.size(); i++)
        if (_devices.isGPU(i))
            s.insert(i);

    return s;
}

set< size_t > OCLinit::accIndexes(void) const
{
    set< size_t > s;

    for (size_t i = 0; i < _devices.size(); i++)
        if (_devices.isACC(i))
            s.insert(i);

    return s;
}

}; // namespace chai_internal
