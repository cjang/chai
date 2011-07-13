// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <string.h>

#include "OCLdevice.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// utility

template <> size_t imgveclen<float>() { return 4; }
template <> size_t imgveclen<double>() { return 2; }

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

    if (CL_SUCCESS == status) {
        index = _ptrs.size();
        _ptrs.push_back(ptr);
        _freePtr.push_back(freePtr);
        _handles.push_back(handle);
        _access.push_back(mode);
        _pinned.push_back(pinned);
        _size.push_back(size);
        _refcnt.push_back(0);
    } else {
        if (freePtr) free(ptr);
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
        if (0 != posix_memalign(static_cast<void**>(&ptr), alignment, size))
            return -1;

    return create(size, mode, pinned, ptr, true);
}

OCLHeapOfMemoryBuffers::OCLHeapOfMemoryBuffers(const cl_context context,
                                               const cl_command_queue queue)
    : _context(context),
      _queue(queue)
{ }

OCLHeapOfMemoryBuffers::~OCLHeapOfMemoryBuffers(void)
{
    waitWrite();
    waitRead();
    for (size_t i = 0; i < _handles.size(); i++) {
        clReleaseMemObject(_handles[i]);
        if (_freePtr[i]) free(_ptrs[i]);
    }
}

size_t OCLHeapOfMemoryBuffers::scavenge(void)
{
    Type<void*>::aligned_vector  newPtrs;
    vector<bool>                 newFreePtr;
    Type<cl_mem>::aligned_vector newHandles;
    vector<const OCLBufferMode*> newAccess;
    vector<bool>                 newPinned;
    vector<size_t>               newSize;
    vector<size_t>               newRefcnt;

    size_t releaseCount = 0;

    for (size_t i = 0; i < _refcnt.size(); i++)
        if (0 == _refcnt[i]) {
            clReleaseMemObject(_handles[i]);
            if (_freePtr[i]) free(_ptrs[i]);
            releaseCount++;
        } else {
            newPtrs.push_back(_ptrs[i]);
            newFreePtr.push_back(_freePtr[i]);
            newHandles.push_back(_handles[i]);
            newAccess.push_back(_access[i]);
            newPinned.push_back(_pinned[i]);
            newSize.push_back(_size[i]);
            newRefcnt.push_back(_refcnt[i]);
        }

    _ptrs = newPtrs;
    _freePtr = newFreePtr;
    _handles = newHandles;
    _access = newAccess;
    _pinned = newPinned;
    _size = newSize;
    _refcnt = newRefcnt;

    return releaseCount;
}

bool OCLHeapOfMemoryBuffers::waitWrite(void)
{
    if (0 == _writeEvents.size()) return true;
    bool isOK = CL_SUCCESS == clWaitForEvents(_writeEvents.size(),
                                              &_writeEvents[0]);
    for (size_t i = 0; i < _writeEvents.size(); i++)
        if (CL_SUCCESS != clReleaseEvent(_writeEvents[i]))
            isOK = false;
    _writeEvents.clear();
    return isOK;
}

bool OCLHeapOfMemoryBuffers::waitRead(void)
{
    if (0 == _readEvents.size()) return true;
    bool isOK = CL_SUCCESS == clWaitForEvents(_readEvents.size(),
                                              &_readEvents[0]);
    for (size_t i = 0; i < _readEvents.size(); i++)
        if (CL_SUCCESS != clReleaseEvent(_readEvents[i]))
            isOK = false;
    _readEvents.clear();
    return isOK;
}

void OCLHeapOfMemoryBuffers::checkout(const size_t index)
{
    _refcnt[index]++;
}

void OCLHeapOfMemoryBuffers::release(const size_t index)
{
    if (_refcnt[index]) _refcnt[index]--;
}

void* OCLHeapOfMemoryBuffers::ptr(const size_t index)
{
    return _ptrs[index];
}

const void* OCLHeapOfMemoryBuffers::ptr(const size_t index) const
{
    return _ptrs[index];
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
                            const OCLImageMode* mode,
                            const bool pinned,
                            void* ptr,
                            const bool freePtr)
{
    cl_image_format format;
    format.image_channel_order = CL_RGBA;
    format.image_channel_data_type = CL_UNSIGNED_INT32;

    cl_int status;
    const cl_mem handle = clCreateImage2D(_context,
                                          mode->flags(pinned
                                                      ? CL_MEM_ALLOC_HOST_PTR
                                                      : CL_MEM_USE_HOST_PTR),
                                          &format,
                                          width,
                                          height,
                                          0,
                                          ptr,
                                          &status);

    int index = -1;

    if (CL_SUCCESS == status) {
        index = _ptrs.size();
        _ptrs.push_back(ptr);
        _freePtr.push_back(freePtr);
        _handles.push_back(handle);
        _access.push_back(mode);
        _pinned.push_back(pinned);
        _width.push_back(width);
        _height.push_back(height);
        _refcnt.push_back(0);
    } else {
        if (freePtr) free(ptr);
    }

    return index;
}

int OCLHeapOfImages::create(const size_t width,
                            const size_t height,
                            const OCLImageMode* mode,
                            const bool pinned,
                            const size_t alignment)
{
    const size_t size = width * height * 4 * sizeof(float);

    void *ptr;
    if (pinned)
        ptr = NULL;
    else
        if (0 != posix_memalign(static_cast<void**>(&ptr), alignment, size))
            return -1;

    return create(width, height, mode, pinned, ptr, true);
}

OCLHeapOfImages::OCLHeapOfImages(const cl_context context,
                                 const cl_command_queue queue)
    : _context(context),
      _queue(queue)
{ }

OCLHeapOfImages::~OCLHeapOfImages(void)
{
    waitWrite();
    waitRead();
    for (size_t i = 0; i < _handles.size(); i++) {
        clReleaseMemObject(_handles[i]);
        if (_freePtr[i]) free(_ptrs[i]);
    }
}

size_t OCLHeapOfImages::scavenge(void)
{
    Type<void*>::aligned_vector  newPtrs;
    vector<bool>                 newFreePtr;
    Type<cl_mem>::aligned_vector newHandles;
    vector<const OCLImageMode*>  newAccess;
    vector<bool>                 newPinned;
    vector<size_t>               newWidth;
    vector<size_t>               newHeight;
    vector<size_t>               newRefcnt;

    size_t releaseCount = 0;

    for (size_t i = 0; i < _refcnt.size(); i++)
        if (0 == _refcnt[i]) {
            clReleaseMemObject(_handles[i]);
            if (_freePtr[i]) free(_ptrs[i]);
            releaseCount++;
        } else {
            newPtrs.push_back(_ptrs[i]);
            newFreePtr.push_back(_freePtr[i]);
            newHandles.push_back(_handles[i]);
            newAccess.push_back(_access[i]);
            newPinned.push_back(_pinned[i]);
            newWidth.push_back(_width[i]);
            newHeight.push_back(_height[i]);
            newRefcnt.push_back(_refcnt[i]);
        }

    _ptrs = newPtrs;
    _freePtr = newFreePtr;
    _handles = newHandles;
    _access = newAccess;
    _pinned = newPinned;
    _width = newWidth;
    _height = newHeight;
    _refcnt = newRefcnt;

    return releaseCount;
}

int OCLHeapOfImages::createWithPointer(const size_t width,  // texel dimensions
                                       const size_t height, // texel dimensions
                                       const OCLImageMode& mode,
                                       void* ptr)
{
    return create(width, height, &mode, false, ptr, false);
}

int OCLHeapOfImages::createWithPointer(const size_t width,  // texel dimensions
                                       const size_t height, // texel dimensions
                                       const OCLImageMode& mode,
                                       const void* ptr)
{
    return create(width, height, &mode, false, const_cast<void*>(ptr), false);
}

bool OCLHeapOfImages::waitWrite(void)
{
    if (0 == _writeEvents.size()) return true;
    bool isOK = CL_SUCCESS == clWaitForEvents(_writeEvents.size(),
                                              &_writeEvents[0]);
    for (size_t i = 0; i < _writeEvents.size(); i++)
        if (CL_SUCCESS != clReleaseEvent(_writeEvents[i]))
            isOK = false;
    _writeEvents.clear();
    return isOK;
}

bool OCLHeapOfImages::waitRead(void)
{
    if (0 == _readEvents.size()) return true;
    bool isOK = CL_SUCCESS == clWaitForEvents(_readEvents.size(),
                                              &_readEvents[0]);
    for (size_t i = 0; i < _readEvents.size(); i++)
        if (CL_SUCCESS != clReleaseEvent(_readEvents[i]))
            isOK = false;
    _readEvents.clear();
    return isOK;
}

void OCLHeapOfImages::checkout(const size_t index)
{
    _refcnt[index]++;
}

void OCLHeapOfImages::release(const size_t index)
{
    if (_refcnt[index]) _refcnt[index]--;
}

void* OCLHeapOfImages::ptr(const size_t index)
{
    return _ptrs[index];
}

const void* OCLHeapOfImages::ptr(const size_t index) const
{
    return _ptrs[index];
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

int OCLHeapOfKernels::create(const vector<string>& source,
                             const string& options,
                             const string& kernelName)
{
    const char *src[source.size()];
    for (size_t i = 0; i < source.size(); i++)
        src[i] = source[i].c_str();

    cl_int status;

    const cl_program program = clCreateProgramWithSource(_context,
                                                         source.size(),
                                                         src,
                                                         NULL,
                                                         &status);
    if (CL_SUCCESS != status) return -1;

    if (CL_SUCCESS != clBuildProgram(program,
                                     1,
                                     &_device,
                                     options.c_str(),
                                     NULL,
                                     NULL)) {
        size_t msgsize = 0;
        char msgbuf[10240];
        memset(msgbuf, 0, sizeof(msgbuf));
        if (CL_SUCCESS == clGetProgramBuildInfo(program,
                                                _device,
                                                CL_PROGRAM_BUILD_LOG,
                                                sizeof(msgbuf),
                                                msgbuf,
                                                &msgsize))
            cerr << msgbuf << endl;
        clReleaseProgram(program);
        return -1;
    }

    const cl_kernel kernel = clCreateKernel(program,
                                            kernelName.c_str(),
                                            &status);
    if (CL_SUCCESS != status) {
        clReleaseProgram(program);
        return -1;
    }

    const int index = _programs.size();
    _programs.push_back(program);
    _kernels.push_back(kernel);
    _source.push_back(source);
    _kernelName.push_back(kernelName);
    _refcnt.push_back(0);
    return index;
}

OCLHeapOfKernels::OCLHeapOfKernels(const cl_device_id device,
                                   const cl_context context,
                                   const cl_command_queue queue)
    : _device(device),
      _context(context),
      _queue(queue)
{ }

OCLHeapOfKernels::~OCLHeapOfKernels(void)
{
    for (size_t i = 0; i < _kernels.size(); i++)
        clReleaseKernel(_kernels[i]);

    for (size_t i = 0; i < _programs.size(); i++)
        clReleaseProgram(_programs[i]);
}

size_t OCLHeapOfKernels::scavenge(void)
{
    Type<cl_program>::aligned_vector newPrograms;
    Type<cl_kernel>::aligned_vector  newKernels;
    vector< vector<string> >         newSource;
    vector<string>                   newKernelName;
    vector<size_t>                   newRefcnt;

    size_t releaseCount = 0;

    for (size_t i = 0; i < _refcnt.size(); i++)
        if (0 == _refcnt[i]) {
            clReleaseKernel(_kernels[i]);
            clReleaseProgram(_programs[i]);
            releaseCount++;
        } else {
            newPrograms.push_back(_programs[i]);
            newKernels.push_back(_kernels[i]);
            newSource.push_back(_source[i]);
            newKernelName.push_back(_kernelName[i]);
            newRefcnt.push_back(_refcnt[i]);
        }

    _programs = newPrograms;
    _kernels = newKernels;
    _source = newSource;
    _kernelName = newKernelName;
    _refcnt = newRefcnt;

    return releaseCount;
}

int OCLHeapOfKernels::create(const string& source,
                             const string& kernelName)
{
    vector<string> src;
    src.push_back(source);
    return create(src, kernelName);
}

int OCLHeapOfKernels::create(const vector<string>& source,
                             const string& kernelName)
{
    for (size_t i = 0; i < _refcnt.size(); i++)
        if (0 == _refcnt[i]
            && source == _source[i]
            && kernelName == _kernelName[i])
            return i;
    return create(source, "", kernelName);
}

bool OCLHeapOfKernels::enqueue(const size_t index,
                               const vector<size_t>& global,
                               const vector<size_t>& local)
{
    const size_t dim = global.size();
    if (dim != local.size()) return false;

    cl_event event;
    if (CL_SUCCESS != clEnqueueNDRangeKernel(_queue,
                                             _kernels[index],
                                             dim,
                                             NULL,
                                             &global[0],
                                             &local[0],
                                             0,
                                             NULL,
                                             &event))
        return false;

    _events.push_back(event);
    return true;
}

bool OCLHeapOfKernels::wait(void)
{
    if (0 == _events.size()) return true;
    bool isOK = CL_SUCCESS == clWaitForEvents(_events.size(), &_events[0]);
    for (size_t i = 0; i < _events.size(); i++)
        if (CL_SUCCESS != clReleaseEvent(_events[i]))
            isOK = false;
    _events.clear();
    return isOK;
}

bool OCLHeapOfKernels::setArg(const size_t index,
                              const size_t argIndex,
                              const cl_mem object)
{
    return CL_SUCCESS == clSetKernelArg(_kernels[index],
                                        argIndex,
                                        sizeof(cl_mem),
                                        &object);
}

void OCLHeapOfKernels::checkout(const size_t index)
{
    if (-1 != index) _refcnt[index]++;
}

void OCLHeapOfKernels::release(const size_t index)
{
    if (-1 != index && _refcnt[index]) _refcnt[index]--;
}

////////////////////////////////////////
// Device

OCLdevice::OCLdevice(OCLinit& base,
                     const size_t deviceIndex)
    : _base(base),
      _deviceIndex(deviceIndex),
      _bufHeap(context(), queue()),
      _imgHeap(context(), queue()),
      _krnlHeap(device(), context(), queue())
{ }

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

void OCLdevice::scavenge(void)
{
    const size_t bufcnt = buffers().scavenge();
    const size_t imgcnt = images().scavenge();
    const size_t krnlcnt = kernels().scavenge();
    cerr << "scavenged membufs=" << bufcnt
              << " imgbufs=" << imgcnt
              << " kernels=" << krnlcnt
              << endl;
}

////////////////////////////////////////
// Kernel

OCLkernel::OCLkernel(OCLdevice& cdev)
    : _heap(cdev.kernels()),
      _index(-1),
      _global(),
      _local(),
      _argIndex(0)
{ }

OCLkernel::OCLkernel(OCLdevice& cdev,
                     const string& kernelName,
                     const string& source)
    : _heap(cdev.kernels()),
      _index(_heap.create(source, kernelName)),
      _global(),
      _local(),
      _argIndex(0)
{
    _heap.checkout(_index);
}

OCLkernel::OCLkernel(OCLdevice& cdev,
                     const string& kernelName,
                     const vector<string>& source)
    : _heap(cdev.kernels()),
      _index(_heap.create(source, kernelName)),
      _global(),
      _local(),
      _argIndex(0)
{
    _heap.checkout(_index);
}

OCLkernel::~OCLkernel(void)
{
    _heap.release(_index);
}

void OCLkernel::build(const string& kernelName,
                      const string& source)
{
    _index = _heap.create(source, kernelName);
    _heap.checkout(_index);
}

void OCLkernel::build(const string& kernelName,
                      const vector<string>& source)
{
    _index = _heap.create(source, kernelName);
    _heap.checkout(_index);
}

size_t OCLkernel::index(void) const
{
    return _index;
}

const vector<size_t>& OCLkernel::global(void) const
{
    return _global;
}

const vector<size_t>& OCLkernel::local(void) const
{
    return _local;
}

void OCLkernel::workDim(const size_t globalDim, const size_t localDim)
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

////////////////////////////////////////
// more utility

// flush memory buffers, images and kernels //

OCLFlush::OCLFlush(void) { }

OCLdevice& operator << (OCLdevice& cdev, const OCLFlush&)
{
    if (!cdev.buffers().waitWrite())
        cerr << "wait on write buffers failed" << endl;
    if (!cdev.images().waitWrite())
        cerr << "wait on write images failed" << endl;
    if (!cdev.kernels().wait())
        cerr << "wait on kernels failed" << endl;
    return cdev;
}

OCLdevice& operator >> (OCLdevice& cdev, const OCLFlush&)
{
    if (!cdev.buffers().waitRead())
        cerr << "wait on read buffers failed" << endl;
    if (!cdev.images().waitRead())
        cerr << "wait on read images failed" << endl;
    return cdev;
}

// kernel execution ////////////////////

OCLdevice& operator << (OCLdevice& cdev, OCLkernel& krnl)
{
    if (!cdev.kernels().enqueue(krnl.index(), krnl.global(), krnl.local()))
        cerr << "enqueue kernel failed" << endl;
    krnl.clearDim();
    krnl.clearArgIndex();
    return cdev;
}

OCLWorkIndex::OCLWorkIndex(const size_t global, const size_t local)
    : _global(global), _local(local) 
{ }

size_t OCLWorkIndex::global(void) const
{
    return _global;
}

size_t OCLWorkIndex::local(void) const
{
    return _local;
}

OCLkernel& operator << (OCLkernel& krnl, const OCLWorkIndex& dims)
{
    krnl.workDim(dims.global(), dims.local());
    return krnl;
}

}; // namespace chai_internal
