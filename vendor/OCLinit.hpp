// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_OCL_INIT_HPP_
#define _CHAI_OCL_INIT_HPP_

#include <CL/cl.h>
#include <set>
#include <string>

#include "MemalignSTL.hpp"

namespace chai_internal {

class OCLinit;

////////////////////////////////////////
// platforms

class OCLplatforms
{
    friend class OCLinit;

    Type<cl_platform_id>::aligned_vector _handles;

    OCLplatforms(void);

public:
    size_t size(void) const;
    cl_platform_id& operator[] (const size_t i);
    const cl_platform_id& operator[] (const size_t i) const;
};

////////////////////////////////////////
// devices

class OCLdevices
{
    friend class OCLinit;

    Type<cl_device_id>::aligned_vector _handles;
    std::vector< size_t >              _platform;
    std::vector< std::string >         _name;
    std::vector< std::string >         _vendor;
    std::vector< size_t >              _maxWorkGroupSize;
    std::vector< bool >                _isCPU;
    std::vector< bool >                _isGPU;
    std::vector< bool >                _isACC;

    void insertDevice(const cl_device_id deviceID,
                      const size_t platformIndex);

    OCLdevices(const OCLplatforms& platforms);

public:
    size_t size(void) const;
    cl_device_id& operator [] (const size_t i);
    const cl_device_id& operator [] (const size_t i) const;

    size_t platform(const size_t i) const;
    std::string name(const size_t i) const;
    std::string vendor(const size_t i) const;
    size_t maxWorkGroupSize(const size_t i) const;
    bool isCPU(const size_t i) const;
    bool isGPU(const size_t i) const;
    bool isACC(const size_t i) const;
};

////////////////////////////////////////
// contexts

class OCLcontexts
{
    friend class OCLinit;

    Type<cl_context>::aligned_vector _handles;

    OCLcontexts(const OCLplatforms& platforms,
                const OCLdevices& devices);

public:
    ~OCLcontexts(void);

    size_t size(void) const;
    cl_context& operator [] (const size_t i);
    const cl_context& operator [] (const size_t i) const;

    // undefined
    OCLcontexts(const OCLcontexts&);
    OCLcontexts& operator = (const OCLcontexts&);
};

////////////////////////////////////////
// queues

class OCLqueues
{
    friend class OCLinit;

    Type<cl_command_queue>::aligned_vector _handles;

    OCLqueues(const OCLdevices& devices,
              const OCLcontexts& contexts);

public:
    ~OCLqueues(void);

    size_t size(void) const;
    cl_command_queue& operator [] (const size_t i);
    const cl_command_queue& operator [] (const size_t i) const;

    // undefined
    OCLqueues(const OCLqueues&);
    OCLqueues& operator = (const OCLqueues&);
};

////////////////////////////////////////
// init

class OCLinit
{
    OCLplatforms _platforms;
    OCLdevices   _devices;
    OCLcontexts  _contexts;
    OCLqueues    _queues;

public:
    OCLinit(void);

    size_t numberDevices(void) const;

    OCLplatforms& platforms(void);
    const OCLplatforms& platforms(void) const;

    OCLdevices& devices(void);
    const OCLdevices& devices(void) const;

    OCLcontexts& contexts(void);
    const OCLcontexts& contexts(void) const;

    OCLqueues& queues(void);
    const OCLqueues& queues(void) const;

    std::set< size_t > cpuIndexes(void) const;
    std::set< size_t > gpuIndexes(void) const;
    std::set< size_t > accIndexes(void) const;

    // undefined
    OCLinit(const OCLinit& other);
    OCLinit& operator = (const OCLinit& other);
};

}; // namespace chai_internal

#endif
