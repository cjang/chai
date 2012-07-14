// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_OCL_DEVICE_HPP_
#define _CHAI_OCL_DEVICE_HPP_

#include <CL/cl.h>
#include <map>
#include <set>
#include <stdint.h>
#include <string>

#include "OCLinit.hpp"
#include "MemalignSTL.hpp"
#include "PrecType.hpp"

namespace chai_internal {

class OCLdevice;

////////////////////////////////////////
// utility

template <typename SCALAR> size_t imgveclen();

////////////////////////////////////////
// access modes

struct OCLAccessMode {
    virtual int flags(const int extra = 0) const = 0;
};

struct OCLBufferMode : public OCLAccessMode { };
struct OCLImageMode : public OCLAccessMode { };

struct OCLReadMode : public OCLBufferMode, public OCLImageMode {
    int flags(const int) const;
};
struct OCLWriteMode : public OCLBufferMode, public OCLImageMode {
    int flags(const int) const;
};
struct OCLReadWriteMode : public OCLBufferMode {
    int flags(const int) const;
};

static const OCLReadMode READ;
static const OCLWriteMode WRITE;
static const OCLReadWriteMode READWRITE;

////////////////////////////////////////
// one queue per device

class OCLDeviceEventQueue
{
    friend class OCLdevice;

    Type<cl_event>::aligned_vector _oldEvents;
    Type<cl_event>::aligned_vector _newEvents;

    OCLDeviceEventQueue(void);

public:
    ~OCLDeviceEventQueue(void);

    const cl_event* getEvents(void) const;
    size_t sizeEvents(void) const;
    void moveEvents(void);

    void pushEvent(const cl_event);
    bool waitEvents(void);
};

////////////////////////////////////////
// heap of memory buffers

class OCLHeapOfMemoryBuffers
{
    friend class OCLdevice;

    const cl_context                  _context;
    const cl_command_queue            _queue;

    Type<void*>::aligned_vector       _ptrs;
    std::vector<bool>                 _freePtr;
    Type<cl_mem>::aligned_vector      _handles;
    std::vector<const OCLBufferMode*> _access;
    std::vector<bool>                 _pinned;
    std::vector<size_t>               _size;
    std::vector<size_t>               _refcnt;

    OCLDeviceEventQueue&              _devQueue;

    int create(const size_t size,
               const OCLBufferMode* mode,
               const bool pinned,
               void* ptr,
               const bool freePtr);

    int create(const size_t size,
               const OCLBufferMode* mode,
               const bool pinned,
               const size_t alignment);

    OCLHeapOfMemoryBuffers(const cl_context context,
                           const cl_command_queue queue,
                           OCLDeviceEventQueue& devQueue);

public:
    ~OCLHeapOfMemoryBuffers(void);

    size_t scavenge(void);

    template <typename SCALAR>
    int createWithPointer(const size_t length,
                          const OCLBufferMode& mode,
                          SCALAR* ptr)
    {
        const size_t size = length * sizeof(SCALAR);
        return create(size, &mode, false, ptr, false);
    }

    template <typename SCALAR>
    int createWithPointer(const size_t length,
                          const OCLBufferMode& mode,
                          const SCALAR* ptr)
    {
        const size_t size = length * sizeof(SCALAR);
        return create(size, &mode, false, const_cast<SCALAR*>(ptr), false);
    }

    template <typename SCALAR>
    int createAllocate(const size_t length,
                       const OCLBufferMode& mode,
                       const size_t alignMult)
    {
        const size_t size = length * sizeof(SCALAR);
        for (size_t i = 0; i < _refcnt.size(); i++)
            if (0 == _refcnt[i]
                && size == _size[i]
                && mode.flags() == _access[i]->flags())
                return i;
        return create(size,
                      &mode,
                      false,
                      alignMult * MEMORY_ALIGNMENT);
    }

    template <typename SCALAR>
    int createAllocate(const size_t length,
                       const OCLBufferMode& mode,
                       const bool pinned,
                       const size_t alignMult)
    {
        const size_t size = length * sizeof(SCALAR);
        for (size_t i = 0; i < _refcnt.size(); i++)
            if (0 == _refcnt[i]
                && size == _size[i]
                && mode.flags() == _access[i]->flags()
                && pinned == _pinned[i])
                return i;
        return create(size,
                      &mode,
                      pinned,
                      alignMult * MEMORY_ALIGNMENT);
    }

    template <typename SCALAR>
    bool enqueueWrite(const size_t index,
                      const size_t offset,
                      const size_t length)
    {
        cl_event event;
        if (CL_SUCCESS != clEnqueueWriteBuffer(_queue,
                                               _handles[index],
                                               CL_FALSE, // non-blocking
                                               offset * sizeof(SCALAR),
                                               length * sizeof(SCALAR),
                                               _ptrs[index],
                                               _devQueue.sizeEvents(),
                                               _devQueue.getEvents(),
                                               &event))
            return false;
        _devQueue.moveEvents();
        _devQueue.pushEvent(event);
        return true;
    }

    template <typename SCALAR>
    bool enqueueRead(const size_t index,
                     const size_t offset,
                     const size_t length)
    {
        cl_event event;
        if (CL_SUCCESS != clEnqueueReadBuffer(_queue,
                                              _handles[index],
                                              CL_FALSE, // non-blocking
                                              offset * sizeof(SCALAR),
                                              length * sizeof(SCALAR),
                                              _ptrs[index],
                                              _devQueue.sizeEvents(),
                                              _devQueue.getEvents(),
                                              &event))
            return false;
        _devQueue.moveEvents();
        _devQueue.pushEvent(event);
        return true;
    }

    void checkout(const size_t index);
    void release(const size_t index);

    void* ptr(const size_t index);
    const void* ptr(const size_t index) const;

    void ownPtr(const size_t index);

    cl_mem& handle(const size_t index);
    const cl_mem& handle(const size_t index) const;
};

////////////////////////////////////////
// heap of images

class OCLHeapOfImages
{
    friend class OCLdevice;

    const cl_context                 _context;
    const cl_command_queue           _queue;

    Type<void*>::aligned_vector      _ptrs;
    std::vector<bool>                _freePtr;
    Type<cl_mem>::aligned_vector     _handles;
    std::vector<const OCLImageMode*> _access;
    std::vector<bool>                _pinned;
    std::vector<size_t>              _width;
    std::vector<size_t>              _height;
    std::vector<size_t>              _refcnt;

    OCLDeviceEventQueue&             _devQueue;

    int create(const size_t width,  // texel dimensions (x4 or x2 for matrix)
               const size_t height, // texel dimensions
               const OCLImageMode& mode,
               const bool pinned,
               void* ptr,
               const bool freePtr,
               const size_t PREC);

    int create(const size_t width,  // texel dimensions (x4 or x2 for matrix)
               const size_t height, // texel dimensions
               const OCLImageMode& mode,
               const bool pinned,
               const size_t alignment,
               const size_t PREC);

    OCLHeapOfImages(const cl_context context,
                    const cl_command_queue queue,
                    OCLDeviceEventQueue& devQueue);

public:
    ~OCLHeapOfImages(void);

    size_t scavenge(void);

    template <typename SCALAR>
    int createWithPointer(const size_t width,  // texel dimensions
                                               //     (x4 or x2 for matrix)
                          const size_t height, // texel dimensions
                          const OCLImageMode& mode,
                          SCALAR* ptr)
    {
        return create( width,
                       height,
                       mode,
                       false,
                       ptr,
                       false,
                       PrecType::getCode<SCALAR>() );
    }

    template <typename SCALAR>
    int createWithPointer(const size_t width,  // texel dimensions
                                               //     (x4 or x2 for matrix)
                          const size_t height, // texel dimensions
                          const OCLImageMode& mode,
                          const SCALAR* ptr)
    {
        return createWithPointer(
                   width,
                   height,
                   mode,
                   const_cast<SCALAR*>(ptr) );
    }

    template <typename SCALAR>
    int createAllocate(const size_t width,  // texel dimensions
                                            //     (x4 or x2 for matrix)
                       const size_t height, // texel dimensions
                       const OCLImageMode& mode,
                       const size_t alignMult)
    {
        for (size_t i = 0; i < _refcnt.size(); i++)
            if (0 == _refcnt[i]
                && width == _width[i]
                && height == _height[i]
                && mode.flags() == _access[i]->flags())
                return i;
        return create( width,
                       height,
                       mode,
                       false,
                       alignMult * imgveclen<SCALAR>() * MEMORY_ALIGNMENT,
                       PrecType::getCode<SCALAR>() );
    }

    template <typename SCALAR>
    int createAllocate(const size_t width,  // texel dimensions
                                            //     (x4 or x2 for matrix)
                       const size_t height, // texel dimensions
                       const OCLImageMode& mode,
                       const bool pinned,
                       const size_t alignMult)
    {
        for (size_t i = 0; i < _refcnt.size(); i++)
            if (0 == _refcnt[i]
                && width == _width[i]
                && height == _height[i]
                && mode.flags() == _access[i]->flags()
                && pinned == _pinned[i])
                return i;
        return create( width,
                       height,
                       mode,
                       pinned,
                       alignMult * imgveclen<SCALAR>() * MEMORY_ALIGNMENT,
                       PrecType::getCode<SCALAR>() );
    }

    template <typename SCALAR>
    bool enqueueWrite(const size_t index,
                      const size_t originWidth,
                      const size_t originHeight,
                      const size_t regionWidth,
                      const size_t regionHeight)
    {
        size_t origin[3], region[3];
        origin[0] = originWidth;
        origin[1] = originHeight;
        origin[2] = 0;
        region[0] = regionWidth;
        region[1] = regionHeight;
        region[2] = 1;
        cl_event event;
        if (CL_SUCCESS != clEnqueueWriteImage(_queue,
                                              _handles[index],
                                              CL_FALSE, // non-blocking
                                              origin,
                                              region,
                                              0,
                                              0,
                                              _ptrs[index],
                                              _devQueue.sizeEvents(),
                                              _devQueue.getEvents(),
                                              &event))
            return false;
        _devQueue.moveEvents();
        _devQueue.pushEvent(event);
        return true;
    }

    template <typename SCALAR>
    bool enqueueRead(const size_t index,
                     const size_t originWidth,
                     const size_t originHeight,
                     const size_t regionWidth,
                     const size_t regionHeight)
    {
        size_t origin[3], region[3];
        origin[0] = originWidth;
        origin[1] = originHeight;
        origin[2] = 0;
        region[0] = regionWidth;
        region[1] = regionHeight;
        region[2] = 1;
        cl_event event;
        if (CL_SUCCESS != clEnqueueReadImage(_queue,
                                             _handles[index],
                                             CL_FALSE, // non-blocking
                                             origin,
                                             region,
                                             0,
                                             0,
                                             _ptrs[index],
                                             _devQueue.sizeEvents(),
                                             _devQueue.getEvents(),
                                             &event))
            return false;
        _devQueue.moveEvents();
        _devQueue.pushEvent(event);
        return true;
    }

    void checkout(const size_t index);
    void release(const size_t index);

    void* ptr(const size_t index);
    const void* ptr(const size_t index) const;

    void ownPtr(const size_t index);

    cl_mem& handle(const size_t index);
    const cl_mem& handle(const size_t index) const;
};

////////////////////////////////////////
// heap of kernels

class OCLHeapOfKernels
{
    friend class OCLdevice;

    const cl_device_id                          _device;
    const cl_context                            _context;
    const cl_command_queue                      _queue;

    // programs
    Type< cl_program >::aligned_vector          _programHandle;
    std::vector< std::vector< std::string > >   _programSource;
    std::map< uint64_t, size_t >                _programHashCode;

    // kernels
    Type< cl_kernel >::aligned_vector           _kernelHandle;
    std::vector< size_t >                       _programIndex;
    std::vector< size_t >                       _kernelRefcnt;
    std::map< std::string, std::set< size_t > > _kernelMap;

    OCLDeviceEventQueue&                        _devQueue;

    int create(const std::vector<std::string>& source,
               const uint64_t sourceHashCode,
               const std::string& options,
               const std::string& kernelName);

    int create(const size_t programIndex,
               const std::string& kernelName);

    OCLHeapOfKernels(const cl_device_id device,
                     const cl_context context,
                     const cl_command_queue queue,
                     OCLDeviceEventQueue& devQueue);

public:
    ~OCLHeapOfKernels(void);

    size_t scavenge(void);

    int createJIT(const std::string& source,
                  const std::string& kernelName);

    int createJIT(const std::vector<std::string>& source,
                  const std::string& kernelName);

    int createCL(const std::string& source,
                 const uint64_t sourceHashCode,
                 const std::string& kernelName);

    int createCL(const std::vector<std::string>& source,
                 const uint64_t sourceHashCode,
                 const std::string& kernelName);

    bool enqueueKernel(const size_t index,
                       const std::vector<size_t>& global,
                       const std::vector<size_t>& local);

    // global memory or image
    bool setArgGlobal(const size_t index,
                      const size_t argIndex,
                      const cl_mem object);

    // local memory
    template <typename SCALAR>
    bool setArgLocal(const size_t index,
                     const size_t argIndex,
                     const size_t length)
    {
        return CL_SUCCESS == clSetKernelArg(
                                 _kernelHandle[index],
                                 argIndex,
                                 length * sizeof(SCALAR),
                                 NULL );
    }

    template <typename SCALAR>
    bool setArgValue(const size_t index,
                     const size_t argIndex,
                     const SCALAR value)
    {
        return CL_SUCCESS == clSetKernelArg(
                                 _kernelHandle[index],
                                 argIndex,
                                 sizeof(SCALAR),
                                 &value );
    }

    void checkout(const size_t index);
    void release(const size_t index);
};

////////////////////////////////////////
// device

class OCLdevice
{
    OCLinit&               _base;
    const size_t           _deviceIndex;

    OCLDeviceEventQueue    _devQueue;
    OCLHeapOfMemoryBuffers _bufHeap;
    OCLHeapOfImages        _imgHeap;
    OCLHeapOfKernels       _krnlHeap;

    bool                   _statusOp;

public:
    OCLdevice(OCLinit& base,
              const size_t deviceIndex);

    cl_platform_id& platform(void);
    const cl_platform_id& platform(void) const;

    cl_device_id& device(void);
    const cl_device_id& device(void) const;

    cl_context& context(void);
    const cl_context& context(void) const;

    cl_command_queue& queue(void);
    const cl_command_queue& queue(void) const;

    OCLHeapOfMemoryBuffers& buffers(void);
    OCLHeapOfImages& images(void);
    OCLHeapOfKernels& kernels(void);

    size_t maxWorkGroupSize(void) const;

    bool statusOp(void) const;
    void statusOp(const bool);

    void scavenge(void);

    bool waitEvents(void);

    size_t deviceIndex(void) const;
};

////////////////////////////////////////
// memory buffer

template <typename SCALAR>
class OCLmembuf
{
    OCLHeapOfMemoryBuffers& _heap;
    const size_t            _index;

    const size_t            _length;

public:
    OCLmembuf(OCLdevice& cdev,
              const size_t length,
              SCALAR* ptr)
        : _heap(cdev.buffers()),
          _index(_heap.createWithPointer<SCALAR>(length, READWRITE, ptr)),
          _length(length)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    OCLmembuf(OCLdevice& cdev,
              const size_t length,
              const SCALAR* ptr)
        : _heap(cdev.buffers()),
          _index(_heap.createWithPointer<SCALAR>(length, READ, ptr)),
          _length(length)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    OCLmembuf(OCLdevice& cdev,
              const size_t length,
              const OCLBufferMode& accessMode,
              const size_t alignMult = 1)
        : _heap(cdev.buffers()),
          _index(_heap.createAllocate<SCALAR>(length,
                                              accessMode,
                                              alignMult)),
          _length(length)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    OCLmembuf(OCLdevice& cdev,
              const size_t length,
              const OCLBufferMode& accessMode,
              const bool pinned,
              const size_t alignMult = 1)
        : _heap(cdev.buffers()),
          _index(_heap.createAllocate<SCALAR>(length,
                                              accessMode,
                                              pinned,
                                              alignMult)),
          _length(length)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    ~OCLmembuf(void) {
        if (-1 != _index) _heap.release(_index);
    }

    size_t length(void) const {
        return _length;
    }

    size_t index(void) const {
        return _index;
    }

    bool isOk(void) const {
        return -1 != index();
    }

    operator SCALAR* (void) {
        return reinterpret_cast<SCALAR*>(_heap.ptr(_index));
    }

    operator const SCALAR* (void) const {
        return reinterpret_cast<const SCALAR*>(_heap.ptr(_index));
    }

    SCALAR& operator[] (const size_t i) {
        return static_cast<SCALAR*>(*this)[i];
    }

    const SCALAR& operator[] (const size_t i) const {
        return static_cast<const SCALAR*>(*this)[i];
    }

    void ownPtr(void) {
        _heap.ownPtr(_index);
    }

    cl_mem& handle(void) {
        return _heap.handle(_index);
    }

    const cl_mem& handle(void) const {
        return _heap.handle(_index);
    }
};

////////////////////////////////////////
// image buffer

template <typename SCALAR>
class OCLimgbuf
{
    OCLHeapOfImages& _heap;
    const size_t     _index;

    const size_t     _width;
    const size_t     _height;

public:
    OCLimgbuf(OCLdevice& cdev,
              const size_t width,  // matrix dimensions
              const size_t height, // matrix dimensions
              SCALAR* ptr)
        : _heap(cdev.images()),
          _index(_heap.createWithPointer(width / imgveclen<SCALAR>(),
                                         height,
                                         WRITE,
                                         ptr)),
          _width(width),
          _height(height)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    OCLimgbuf(OCLdevice& cdev,
              const size_t width,  // matrix dimensions
              const size_t height, // matrix dimensions
              const SCALAR* ptr)
        : _heap(cdev.images()),
          _index(_heap.createWithPointer(width / imgveclen<SCALAR>(),
                                         height,
                                         READ,
                                         ptr)),
          _width(width),
          _height(height)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    OCLimgbuf(OCLdevice& cdev,
              const size_t width,  // matrix dimensions
              const size_t height, // matrix dimensions
              const OCLImageMode& accessMode,
              const size_t alignMult = 1)
        : _heap(cdev.images()),
          _index(_heap.createAllocate<SCALAR>(width / imgveclen<SCALAR>(),
                                              height,
                                              accessMode,
                                              alignMult)),
          _width(width),
          _height(height)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    OCLimgbuf(OCLdevice& cdev,
           const size_t width,  // matrix dimensions
           const size_t height, // matrix dimensions
           const OCLImageMode& accessMode,
           const bool pinned,
           const size_t alignMult = 1)
        : _heap(cdev.images()),
          _index(_heap.createAllocate<SCALAR>(width / imgveclen<SCALAR>(),
                                              height,
                                              accessMode,
                                              pinned,
                                              alignMult)),
          _width(width),
          _height(height)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    ~OCLimgbuf(void) {
        if (-1 != _index) _heap.release(_index);
    }

    size_t width(void) const {
        return _width;
    }

    size_t height(void) const {
        return _height;
    }

    size_t index(void) const {
        return _index;
    }

    bool isOk(void) const {
        return -1 != index();
    }

    operator SCALAR* (void) {
        return reinterpret_cast<SCALAR*>(_heap.ptr(_index));
    }

    operator const SCALAR* (void) const {
        return reinterpret_cast<const SCALAR*>(_heap.ptr(_index));
    }

    SCALAR& operator[] (const size_t i) {
        return static_cast<SCALAR*>(*this)[i];
    }

    const SCALAR& operator[] (const size_t i) const {
        return static_cast<const SCALAR*>(*this)[i];
    }

    void ownPtr(void) {
        _heap.ownPtr(_index);
    }

    cl_mem& handle(void) {
        return _heap.handle(_index);
    }

    const cl_mem& handle(void) const {
        return _heap.handle(_index);
    }
};

////////////////////////////////////////
// kernel

class OCLkernel
{
    OCLHeapOfKernels&   _heap;
    size_t              _index;

    std::vector<size_t> _global;
    std::vector<size_t> _local;

    size_t              _argIndex;

    bool                _statusOp;

public:
    OCLkernel(OCLdevice& cdev);
    ~OCLkernel(void);

    void buildJIT(const std::string& kernelName,
                  const std::string& source);
    void buildJIT(const std::string& kernelName,
                  const std::vector<std::string>& source);

    void buildCL(const std::string& kernelName,
                 const std::string& source,
                 const uint64_t sourceHashCode);
    void buildCL(const std::string& kernelName,
                 const std::vector<std::string>& source,
                 const uint64_t sourceHashCode);

    size_t index(void) const;

    bool isOk(void) const;

    const std::vector<size_t>& global(void) const;
    const std::vector<size_t>& local(void) const;
    void workDim(const size_t globalDim, const size_t localDim);
    void clearDim(void);

    void clearArgIndex(void);

    template <typename SCALAR>
    bool setArgGlobal(const OCLmembuf<SCALAR>& mb, const bool lazy = false) {
        return _heap.setArgGlobal(_index, _argIndex++, mb.handle());
    }

    template <typename SCALAR>
    bool setArgGlobal(const OCLimgbuf<SCALAR>& ib) {
        return _heap.setArgGlobal(_index, _argIndex++, ib.handle());
    }

    template <typename SCALAR>
    bool setArgLocal(const size_t length) {
        return _heap.setArgLocal<SCALAR>(_index, _argIndex++, length);
    }

    template <typename SCALAR>
    bool setArgValue(const SCALAR value) {
        return _heap.setArgValue<SCALAR>(_index, _argIndex++, value);
    }

    bool statusOp(void) const;
    void statusOp(const bool);
};

////////////////////////////////////////
// more utility

// flush memory buffers, images and kernels //

struct OCLFlush { OCLFlush(); };
static const OCLFlush FLUSH;

OCLdevice& operator << (OCLdevice& cdev, const OCLFlush&);
OCLdevice& operator >> (OCLdevice& cdev, const OCLFlush&);

// PCIe bus data transfer //////////////

template <typename SCALAR>
OCLdevice& operator << (OCLdevice& cdev,
                        OCLmembuf<SCALAR>& mb)
{
    cdev.statusOp(cdev.buffers()
                      .enqueueWrite<SCALAR>(mb.index(),
                                            0,
                                            mb.length()));
    return cdev;
}

template <typename SCALAR>
OCLdevice& operator >> (OCLdevice& cdev,
                        OCLmembuf<SCALAR>& mb)
{
    cdev.statusOp(cdev.buffers()
                      .enqueueRead<SCALAR>(mb.index(),
                                           0,
                                           mb.length()));
    return cdev;
}

template <typename SCALAR>
OCLdevice& operator << (OCLdevice& cdev,
                        OCLimgbuf<SCALAR>& ib)
{
    cdev.statusOp(cdev.images()
                      .enqueueWrite<SCALAR>(ib.index(),
                                            0,
                                            0,
                                            ib.width() / imgveclen<SCALAR>(),
                                            ib.height()));
    return cdev;
}

template <typename SCALAR>
OCLdevice& operator >> (OCLdevice& cdev,
                        OCLimgbuf<SCALAR>& ib)
{
    cdev.statusOp(cdev.images()
                      .enqueueRead<SCALAR>(ib.index(),
                                           0,
                                           0,
                                           ib.width() / imgveclen<SCALAR>(),
                                           ib.height()));
    return cdev;
}

// kernel execution ////////////////////

OCLdevice& operator << (OCLdevice& cdev, OCLkernel& krnl);

class OCLWorkIndex
{
    const size_t _global;
    const size_t _local;

public:
    OCLWorkIndex(const size_t global,
                 const size_t local);

    size_t global(void) const;
    size_t local(void) const;
};

OCLkernel& operator << (OCLkernel& krnl, const OCLWorkIndex& dims);

// set kernel arguments ////////////////

template <typename SCALAR>
OCLkernel& operator << (OCLkernel& ckernel,
                        OCLmembuf<SCALAR>& mb)
{
    ckernel.statusOp(ckernel.setArgGlobal(mb));
    return ckernel;
}

template <typename SCALAR>
OCLkernel& operator << (OCLkernel& ckernel,
                        OCLimgbuf<SCALAR>& ib)
{
    ckernel.statusOp(ckernel.setArgGlobal(ib));
    return ckernel;
}

template <typename SCALAR>
class OCLLocalbuf
{
    const size_t _length;

public:
    OCLLocalbuf(const size_t length)
        : _length(length) { }

    size_t length(void) const { return _length; }
};

template <typename SCALAR>
OCLkernel& operator << (OCLkernel& ckernel,
                        OCLLocalbuf<SCALAR>& lb)
{
    ckernel.statusOp(ckernel.setArgLocal<SCALAR>(lb.length()));
    return ckernel;
}

template <typename SCALAR>
OCLkernel& operator << (OCLkernel& ckernel,
                        const SCALAR value)
{
    ckernel.statusOp(ckernel.setArgValue<SCALAR>(value));
    return ckernel;
}

}; // namespace chai_internal

#endif
