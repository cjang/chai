// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include <string.h>

#include "Logger.hpp"
#include "OCLdevice.hpp"
#include "TEA.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// utility

template <> size_t imgveclen<float>() { return 4; }
template <> size_t imgveclen<double>() { return 2; }
template <> size_t imgveclen<int32_t>() { return 4; }
template <> size_t imgveclen<uint32_t>() { return 4; }

////////////////////////////////////////
// AccessMode

int OCLReadMode::flags(const int extra) const
{
    return extra
           ? CL_MEM_READ_ONLY | extra
           : CL_MEM_READ_ONLY;
}

int OCLWriteMode::flags(const int extra) const
{
    return extra
           ? CL_MEM_WRITE_ONLY | extra
           : CL_MEM_WRITE_ONLY;
}

int OCLReadWriteMode::flags(const int extra) const
{
    return extra
           ? CL_MEM_READ_WRITE | extra
           : CL_MEM_READ_WRITE;
}

////////////////////////////////////////
// one queue per device

OCLDeviceEventQueue::OCLDeviceEventQueue(void)
    : _oldEvents(),
      _newEvents() { }

OCLDeviceEventQueue::~OCLDeviceEventQueue(void)
{
    waitEvents();
}

const cl_event* OCLDeviceEventQueue::getEvents(void) const
{
    return 0 == _newEvents.size() ? NULL
                                  : &_newEvents[0];
}

size_t OCLDeviceEventQueue::sizeEvents(void) const
{
    return _newEvents.size();
}

void OCLDeviceEventQueue::moveEvents(void)
{
    for (size_t i = 0; i < _newEvents.size(); i++)
    {
        _oldEvents.push_back(_newEvents[i]);
    }

    _newEvents.clear();
}

void OCLDeviceEventQueue::pushEvent(const cl_event event)
{
    _newEvents.push_back(event);
}

bool OCLDeviceEventQueue::waitEvents(void)
{
    if (0 == _newEvents.size())
        return true;

    bool isOK = CL_SUCCESS == clWaitForEvents(sizeEvents(),
                                              getEvents());

    for (size_t i = 0; i < _oldEvents.size(); i++)
        if (CL_SUCCESS != clReleaseEvent(_oldEvents[i]))
            isOK = false;

    for (size_t i = 0; i < _newEvents.size(); i++)
        if (CL_SUCCESS != clReleaseEvent(_newEvents[i]))
            isOK = false;

    _oldEvents.clear();
    _newEvents.clear();

    return isOK;
}

////////////////////////////////////////
// HeapOfMemoryBuffers

int OCLHeapOfMemoryBuffers::create(const size_t size,
                                   const OCLBufferMode* mode,
                                   const bool pinned,
                                   void* ptr,
                                   const bool freePtr)
{
    cl_int status;
    const cl_mem handle = clCreateBuffer(_context,
                                         mode->flags(pinned
                                                     ? CL_MEM_ALLOC_HOST_PTR
                                                     : CL_MEM_USE_HOST_PTR),
                                         size,
                                         ptr,
                                         &status);

    int index = -1;

    if (CL_SUCCESS == status)
    {
        index = _ptrs.size();
        _ptrs.push_back(ptr);
        _freePtr.push_back(freePtr);
        _handles.push_back(handle);
        _access.push_back(mode);
        _pinned.push_back(pinned);
        _size.push_back(size);
        _refcnt.push_back(0);
    }
    else
    {
        if (freePtr)
            free(ptr);
    }

    return index;
}

int OCLHeapOfMemoryBuffers::create(const size_t size,
                                   const OCLBufferMode* mode,
                                   const bool pinned,
                                   const size_t alignment)
{
    void *ptr;
    if (pinned)
        ptr = NULL;
    else
        if (0 != posix_memalign(static_cast<void**>(&ptr),
                                alignment,
                                size))
            return -1;

    return create(size, mode, pinned, ptr, true);
}

OCLHeapOfMemoryBuffers::OCLHeapOfMemoryBuffers(const cl_context context,
                                               const cl_command_queue queue,
                                               OCLDeviceEventQueue& devQueue)
    : _context(context),
      _queue(queue),
      _devQueue(devQueue) { }

OCLHeapOfMemoryBuffers::~OCLHeapOfMemoryBuffers(void)
{
    _devQueue.waitEvents();

    for (size_t i = 0; i < _handles.size(); i++)
    {
        clReleaseMemObject(_handles[i]);
        if (_freePtr[i])
            free(_ptrs[i]);
    }
}

size_t OCLHeapOfMemoryBuffers::scavenge(void)
{
    size_t totalCount = 0;

    for (vector< size_t >::const_iterator
         it = _refcnt.begin();
         it != _refcnt.end();
         it++)
    {
        totalCount += *it;
    }

    if (0 == totalCount)
    {
        for (size_t i = 0; i < _refcnt.size(); i++)
        {
            clReleaseMemObject(_handles[i]);
            if (_freePtr[i])
                free(_ptrs[i]);
        }

        _ptrs.clear();
        _freePtr.clear();
        _handles.clear();
        _access.clear();
        _pinned.clear();
        _size.clear();
        _refcnt.clear();
    }

    return totalCount;
}

void OCLHeapOfMemoryBuffers::checkout(const size_t index)
{
    _refcnt[index]++;
}

void OCLHeapOfMemoryBuffers::release(const size_t index)
{
    if (_refcnt[index])
        _refcnt[index]--;
}

void* OCLHeapOfMemoryBuffers::ptr(const size_t index)
{
    return _ptrs[index];
}

const void* OCLHeapOfMemoryBuffers::ptr(const size_t index) const
{
    return _ptrs[index];
}

void OCLHeapOfMemoryBuffers::ownPtr(const size_t index)
{
    _freePtr[index] = true;
}

cl_mem& OCLHeapOfMemoryBuffers::handle(const size_t index)
{
    return _handles[index];
}

const cl_mem& OCLHeapOfMemoryBuffers::handle(const size_t index) const
{
    return _handles[index];
}

////////////////////////////////////////
// HeapOfImages

int OCLHeapOfImages::create(const size_t width,
                            const size_t height,
                            const OCLImageMode& mode,
                            const bool pinned,
                            void* ptr,
                            const bool freePtr,
                            const size_t precTypeSizeCode)
{
    cl_image_format format;

    format.image_channel_order = CL_RGBA;

    switch (precTypeSizeCode)
    {
        case (PrecType::UInt32) :
            format.image_channel_data_type = CL_UNSIGNED_INT32;
            break;

        case (PrecType::Int32) :
            format.image_channel_data_type = CL_SIGNED_INT32;
            break;

        case (PrecType::Float) :
            format.image_channel_data_type = CL_FLOAT;
            break;

        case (PrecType::Double) :
            format.image_channel_data_type = CL_UNSIGNED_INT32;
            break;
    }

    cl_int status;
    const cl_mem handle = clCreateImage2D(_context,
                                          mode.flags(pinned
                                                     ? CL_MEM_ALLOC_HOST_PTR
                                                     : CL_MEM_USE_HOST_PTR),
                                          &format,
                                          width,
                                          height,
                                          0,
                                          ptr,
                                          &status);

    int index = -1;

    if (CL_SUCCESS == status)
    {
        index = _ptrs.size();
        _ptrs.push_back(ptr);
        _freePtr.push_back(freePtr);
        _handles.push_back(handle);
        _access.push_back(&mode);
        _pinned.push_back(pinned);
        _width.push_back(width);
        _height.push_back(height);
        _refcnt.push_back(0);
    }
    else
    {
        if (freePtr)
            free(ptr);
    }

    return index;
}

int OCLHeapOfImages::create(const size_t width,
                            const size_t height,
                            const OCLImageMode& mode,
                            const bool pinned,
                            const size_t alignment,
                            const size_t precTypeSizeCode)
{
    // each texel is 32 bits in a 128 bit RGBA quad
    const size_t size = width * height * 4 * sizeof(uint32_t);

    void *ptr;
    if (pinned)
        ptr = NULL;
    else
        if (0 != posix_memalign(static_cast<void**>(&ptr),
                                alignment,
                                size))
            return -1;

    return create(width, height, mode, pinned, ptr, true, precTypeSizeCode);
}

OCLHeapOfImages::OCLHeapOfImages(const cl_context context,
                                 const cl_command_queue queue,
                                 OCLDeviceEventQueue& devQueue)
    : _context(context),
      _queue(queue),
      _devQueue(devQueue) { }

OCLHeapOfImages::~OCLHeapOfImages(void)
{
    _devQueue.waitEvents();

    for (size_t i = 0; i < _handles.size(); i++)
    {
        clReleaseMemObject(_handles[i]);
        if (_freePtr[i])
            free(_ptrs[i]);
    }
}

size_t OCLHeapOfImages::scavenge(void)
{
    size_t totalCount = 0;

    for (vector< size_t >::const_iterator
         it = _refcnt.begin();
         it != _refcnt.end();
         it++)
    {
        totalCount += *it;
    }

    if (0 == totalCount)
    {
        for (size_t i = 0; i < _refcnt.size(); i++)
        {
            clReleaseMemObject(_handles[i]);
            if (_freePtr[i])
                free(_ptrs[i]);
        }

        _ptrs.clear();
        _freePtr.clear();
        _handles.clear();
        _access.clear();
        _pinned.clear();
        _width.clear();
        _height.clear();
        _refcnt.clear();
    }

    return totalCount;
}

void OCLHeapOfImages::checkout(const size_t index)
{
    _refcnt[index]++;
}

void OCLHeapOfImages::release(const size_t index)
{
    if (_refcnt[index])
        _refcnt[index]--;
}

void* OCLHeapOfImages::ptr(const size_t index)
{
    return _ptrs[index];
}

const void* OCLHeapOfImages::ptr(const size_t index) const
{
    return _ptrs[index];
}

void OCLHeapOfImages::ownPtr(const size_t index)
{
    _freePtr[index] = true;
}

cl_mem& OCLHeapOfImages::handle(const size_t index)
{
    return _handles[index];
}

const cl_mem& OCLHeapOfImages::handle(const size_t index) const
{
    return _handles[index];
}

////////////////////////////////////////
// HeapOfKernels

int OCLHeapOfKernels::create( const vector<string>& source,
                              const uint64_t sourceHashCode,
                              const string& options,
                              const string& kernelName )
{
    const char *src[source.size()];
    for (size_t i = 0; i < source.size(); i++)
        src[i] = source[i].c_str();

    cl_int status;

    const cl_program program = clCreateProgramWithSource(
                                   _context,
                                   source.size(),
                                   src,
                                   NULL,
                                   &status );

    if (CL_SUCCESS != status)
    {
#ifdef __LOGGING_ENABLED__
        stringstream ss;
        ss << "clCreateProgramWithSource failed for " << kernelName;
        LOGGER(ss.str())
#endif

        return -1;
    }

    if (CL_SUCCESS != clBuildProgram(
                          program,
                          1,
                          &_device,
                          options.c_str(),
                          NULL,
                          NULL ))
    {
        size_t msgsize = 0;

        char msgbuf[10240];
        memset(msgbuf, 0, sizeof(msgbuf));

        if (CL_SUCCESS == clGetProgramBuildInfo(
                              program,
                              _device,
                              CL_PROGRAM_BUILD_LOG,
                              sizeof(msgbuf),
                              msgbuf,
                              &msgsize ))
        {
            LOGGER(msgbuf)
        }
#ifdef __LOGGING_ENABLED__
        else
        {
            stringstream ss;
            ss << "clBuildProgram failed for " << kernelName;
            LOGGER(ss.str())
        }
#endif

        clReleaseProgram(program);

        return -1;
    }

    const cl_kernel kernel = clCreateKernel(
                                 program,
                                 kernelName.c_str(),
                                 &status );

    if (CL_SUCCESS != status)
    {
        clReleaseProgram(program);
        return -1;
    }

    const size_t progIdx = _programHandle.size();
    const int krnlIdx = _kernelHandle.size();

    _programHandle.push_back(program);
    _programSource.push_back(source);
    if (sourceHashCode)
        _programHashCode[ sourceHashCode ] = progIdx;

    _kernelHandle.push_back(kernel);
    _programIndex.push_back(progIdx);
    _kernelRefcnt.push_back(0);
    _kernelMap[kernelName].insert(krnlIdx);

    return krnlIdx;
}

int OCLHeapOfKernels::create( const size_t progIdx,
                              const string& kernelName )
{
    cl_int status;

    const cl_kernel kernel = clCreateKernel(
                                 _programHandle[progIdx],
                                 kernelName.c_str(),
                                 &status );

    if (CL_SUCCESS != status)
        return -1;

    const int krnlIdx = _kernelHandle.size();

    _kernelHandle.push_back(kernel);
    _programIndex.push_back(progIdx);
    _kernelRefcnt.push_back(0);
    _kernelMap[kernelName].insert(krnlIdx);

    return krnlIdx;
}

OCLHeapOfKernels::OCLHeapOfKernels( const cl_device_id device,
                                    const cl_context context,
                                    const cl_command_queue queue,
                                    OCLDeviceEventQueue& devQueue )
    : _device(device),
      _context(context),
      _queue(queue),
      _devQueue(devQueue) { }

OCLHeapOfKernels::~OCLHeapOfKernels(void)
{
    _devQueue.waitEvents();

    for (size_t i = 0; i < _kernelHandle.size(); i++)
        clReleaseKernel(_kernelHandle[i]);

    for (size_t i = 0; i < _programHandle.size(); i++)
        clReleaseProgram(_programHandle[i]);
}

size_t OCLHeapOfKernels::scavenge(void)
{
    Type< cl_program >::aligned_vector newProgramHandle;
    vector< vector< string > >         newProgramSource;

    Type< cl_kernel >::aligned_vector  newKernelHandle;
    vector< size_t >                   newProgramIndex;
    vector< size_t >                   newKernelRefcnt;
    map< string, set< size_t > >       newKernelMap;

    size_t releaseCount = 0;

    vector< size_t > progRefcnt;
    for (size_t i = 0; i < _programHandle.size(); i++)
        progRefcnt.push_back(0);

    vector< size_t > newKrnlIdx;
    for (size_t i = 0; i < _kernelHandle.size(); i++)
    {
        newKrnlIdx.push_back(newKernelHandle.size());

        const size_t kernelCount = _kernelRefcnt[i];

        progRefcnt[ _programIndex[i] ] += kernelCount;

        if (0 == kernelCount)
        {
            clReleaseKernel(_kernelHandle[i]);
            releaseCount++;
        }
        else
        {
            newKernelHandle.push_back(_kernelHandle[i]);
            newProgramIndex.push_back(_programIndex[i]);
            newKernelRefcnt.push_back(kernelCount);
        }
    }

    vector< size_t > newProgIdx;
    set< size_t > releaseProgIdx;
    for (size_t i = 0; i < _programHandle.size(); i++)
    {
        newProgIdx.push_back(newProgramHandle.size());

        if (0 == progRefcnt[i])
        {
            clReleaseProgram(_programHandle[i]);
            releaseProgIdx.insert(i);
            releaseCount++;
        }
        else
        {
            newProgramHandle.push_back(_programHandle[i]);
            newProgramSource.push_back(_programSource[i]);
        }
    }

    for (size_t i = 0; i < newProgramIndex.size(); i++)
    {
        const size_t idx = newProgIdx[ newProgramIndex[i] ];
        newProgramIndex[i] = idx;
    }

    set< uint64_t > releaseProgramHashCode;
    for (map< uint64_t, size_t >::const_iterator
         it = _programHashCode.begin();
         it != _programHashCode.end();
         it++)
    {
        const size_t oldProgramIndex = (*it).second;

        if (releaseProgIdx.count(oldProgramIndex))
        {
            releaseProgramHashCode.insert((*it).first);
        }
    }

    for (set< uint64_t >::const_iterator
         it = releaseProgramHashCode.begin();
         it != releaseProgramHashCode.end();
         it++)
    {
        _programHashCode.erase(*it);
    }

    for (map< string, set< size_t > >::const_iterator
         it = _kernelMap.begin();
         it != _kernelMap.end();
         it++)
    {
        const string kernelName = (*it).first;

        for (set< size_t >::const_iterator
             jt = (*it).second.begin();
             jt != (*it).second.end();
             jt++)
        {
            newKernelMap[ kernelName ].insert( newKrnlIdx[*jt] );
        }
    } 

    _programHandle = newProgramHandle;
    _programSource = newProgramSource;

    _kernelHandle = newKernelHandle;
    _programIndex = newProgramIndex;
    _kernelRefcnt = newKernelRefcnt;
    _kernelMap = newKernelMap;

    return releaseCount;
}

int OCLHeapOfKernels::createJIT( const string& source,
                                 const string& kernelName )
{
    vector< string > src;
    src.push_back(source);
    return createJIT(src, kernelName);
}

int OCLHeapOfKernels::createJIT( const vector< string >& source,
                                 const string& kernelName )
{
    int retIdx = -1;

    if (_kernelMap.count(kernelName))
    {
        // kernel by this name already exists, may recycle programs

        size_t matchProgIdx = -1;

        for (set< size_t >::const_iterator
             it = _kernelMap[kernelName].begin();
             it != _kernelMap[kernelName].end();
             it++)
        {
            const bool available = (0 == _kernelRefcnt[*it]);
            const size_t progIdx = _programIndex[*it];
            const bool krnlMatch = (source == _programSource[progIdx]);

            if (krnlMatch)
            {
                if (available)
                {
                    return *it;
                }
                else
                {
                    matchProgIdx = progIdx;
                }
            }
        }

        retIdx = ( -1 == matchProgIdx
                       ? create(source, 0, "", kernelName)
                       : create(matchProgIdx, kernelName) );
    }
    else
    {
        // never seen this kernel name before, new program and kernel

        retIdx = create(source, 0, "", kernelName);
    }

    if (-1 != retIdx)
        _kernelMap[kernelName].insert(retIdx);

    return retIdx;
}

int OCLHeapOfKernels::createCL( const string& source,
                                const uint64_t sourceHashCode,
                                const string& kernelName )
{
    vector< string > src;
    src.push_back(source);
    return createCL(src, sourceHashCode, kernelName);
}

int OCLHeapOfKernels::createCL( const vector< string >& source,
                                const uint64_t sourceHashCode,
                                const string& kernelName )
{
    int retIdx = -1;

    if (_programHashCode.count(sourceHashCode))
    {
        // program already exists

        const size_t matchProgIdx = _programHashCode[sourceHashCode];

        retIdx = create(matchProgIdx, kernelName);
    }
    else
    {
        // never seen this program before

        retIdx = create(source, sourceHashCode, "", kernelName);
    }

    if (-1 != retIdx)
        _kernelMap[kernelName].insert(retIdx);

    return retIdx;
}

bool OCLHeapOfKernels::enqueueKernel( const size_t index,
                                      const vector< size_t >& global,
                                      const vector< size_t >& local )
{
    const size_t dim = global.size();

    if (dim != local.size())
        return false;

    cl_event event;
    if (CL_SUCCESS == clEnqueueNDRangeKernel(
                          _queue,
                          _kernelHandle[index],
                          dim,
                          NULL,
                          &global[0],
                          &local[0],
                          _devQueue.sizeEvents(),
                          _devQueue.getEvents(),
                          &event ))
    {
        _devQueue.moveEvents();
        _devQueue.pushEvent(event);
        return true;
    }
    else
    {
        return false;
    }
}

bool OCLHeapOfKernels::setArgGlobal( const size_t index,
                                     const size_t argIndex,
                                     const cl_mem object )
{
    return CL_SUCCESS == clSetKernelArg(
                             _kernelHandle[index],
                             argIndex,
                             sizeof(cl_mem),
                             &object );
}

void OCLHeapOfKernels::checkout(const size_t index)
{
    if (-1 != index)
    {
        _kernelRefcnt[index]++;
    }
}

void OCLHeapOfKernels::release(const size_t index)
{
    if (-1 != index && _kernelRefcnt[index])
    {
        _kernelRefcnt[index]--;
    }
}

////////////////////////////////////////
// Device

OCLdevice::OCLdevice(OCLinit& base,
                     const size_t deviceIndex)
    : _base(base),
      _deviceIndex(deviceIndex),
      _devQueue(),
      _bufHeap(context(), queue(), _devQueue),
      _imgHeap(context(), queue(), _devQueue),
      _krnlHeap(device(), context(), queue(), _devQueue),
      _statusOp(false) { }

cl_platform_id& OCLdevice::platform(void)
{
    return _base.platforms()[_base.devices().platform(_deviceIndex)];
}

const cl_platform_id& OCLdevice::platform(void) const
{
    return _base.platforms()[_base.devices().platform(_deviceIndex)];
}

cl_device_id& OCLdevice::device(void)
{
    return _base.devices()[_deviceIndex];
}

const cl_device_id& OCLdevice::device(void) const
{
    return _base.devices()[_deviceIndex];
}

cl_context& OCLdevice::context(void)
{
    return _base.contexts()[_deviceIndex];
}

const cl_context& OCLdevice::context(void) const
{
    return _base.contexts()[_deviceIndex];
}

cl_command_queue& OCLdevice::queue(void)
{
    return _base.queues()[_deviceIndex];
}

const cl_command_queue& OCLdevice::queue(void) const
{
    return _base.queues()[_deviceIndex];
}

OCLHeapOfMemoryBuffers& OCLdevice::buffers(void)
{
    return _bufHeap;
}

OCLHeapOfImages& OCLdevice::images(void)
{
    return _imgHeap;
}

OCLHeapOfKernels& OCLdevice::kernels(void)
{
    return _krnlHeap;
}

size_t OCLdevice::maxWorkGroupSize(void) const
{
    return _base.devices().maxWorkGroupSize(_deviceIndex);
}

bool OCLdevice::statusOp(void) const
{
    return _statusOp;
}

void OCLdevice::statusOp(const bool flag)
{
    _statusOp = flag;
}

void OCLdevice::scavenge(void)
{
    const size_t bufcnt = buffers().scavenge();
    const size_t imgcnt = images().scavenge();
    const size_t krnlcnt = kernels().scavenge();
}

bool OCLdevice::waitEvents(void)
{
    return _devQueue.waitEvents();
}

size_t OCLdevice::deviceIndex(void) const
{
    return _deviceIndex;
}

////////////////////////////////////////
// Kernel

OCLkernel::OCLkernel(OCLdevice& cdev)
    : _heap(cdev.kernels()),
      _index(-1),
      _global(),
      _local(),
      _argIndex(0),
      _statusOp(false) { }

OCLkernel::~OCLkernel(void)
{
    _heap.release(_index);
}

void OCLkernel::buildJIT(const string& kernelName,
                         const string& source)
{
    _index = _heap.createJIT(source, kernelName);
    _heap.checkout(_index);
}

void OCLkernel::buildJIT(const string& kernelName,
                         const vector<string>& source)
{
    _index = _heap.createJIT(source, kernelName);
    _heap.checkout(_index);
}

void OCLkernel::buildCL(const string& kernelName,
                        const string& source,
                        const uint64_t sourceHashCode)
{
    _index = _heap.createCL(source, sourceHashCode, kernelName);
    _heap.checkout(_index);
}

void OCLkernel::buildCL(const string& kernelName,
                        const vector<string>& source,
                        const uint64_t sourceHashCode)
{
    _index = _heap.createCL(source, sourceHashCode, kernelName);
    _heap.checkout(_index);
}

size_t OCLkernel::index(void) const
{
    return _index;
}

bool OCLkernel::isOk(void) const
{
    return -1 != _index;
}

const vector<size_t>& OCLkernel::global(void) const
{
    return _global;
}

const vector<size_t>& OCLkernel::local(void) const
{
    return _local;
}

void OCLkernel::workDim(const size_t globalDim,
                        const size_t localDim)
{
    _global.push_back(globalDim);
    _local.push_back(localDim);
}

void OCLkernel::clearDim(void)
{
    _global.clear();
    _local.clear();
}

void OCLkernel::clearArgIndex(void)
{
    _argIndex = 0;
}

bool OCLkernel::statusOp(void) const
{
    return _statusOp;
}

void OCLkernel::statusOp(const bool flag)
{
    _statusOp = flag;
}

////////////////////////////////////////
// more utility

// flush memory buffers, images and kernels //

OCLFlush::OCLFlush(void) { }

OCLdevice& operator << (OCLdevice& cdev,
                        const OCLFlush&)
{
    cdev.statusOp(cdev.waitEvents());
    return cdev;
}

OCLdevice& operator >> (OCLdevice& cdev,
                        const OCLFlush&)
{
    cdev.statusOp(cdev.waitEvents());
    return cdev;
}

// kernel execution ////////////////////

OCLdevice& operator << (OCLdevice& cdev,
                        OCLkernel& ckernel)
{
    cdev.statusOp(cdev.kernels()
                      .enqueueKernel(ckernel.index(),
                                     ckernel.global(),
                                     ckernel.local()));
    ckernel.clearDim();
    ckernel.clearArgIndex();
    return cdev;
}

OCLWorkIndex::OCLWorkIndex(const size_t global,
                           const size_t local)
    : _global(global),
      _local(local) { }

size_t OCLWorkIndex::global(void) const
{
    return _global;
}

size_t OCLWorkIndex::local(void) const
{
    return _local;
}

OCLkernel& operator << (OCLkernel& ckernel,
                        const OCLWorkIndex& dims)
{
    ckernel.workDim(dims.global(),
                    dims.local());
    return ckernel;
}

}; // namespace chai_internal
