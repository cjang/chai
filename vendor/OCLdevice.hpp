// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_OCL_DEVICE_HPP_
#define _CHAI_OCL_DEVICE_HPP_

#include <CL/cl.h>
#include <iostream>
#include <string>

#include "OCLinit.hpp"
#include "MemalignSTL.hpp"

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

    Type<cl_event>::aligned_vector    _writeEvents;
    Type<cl_event>::aligned_vector    _readEvents;

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
                           const cl_command_queue queue);

public:
    ~OCLHeapOfMemoryBuffers(void);

    size_t scavenge(void);

    template <typename SCALAR>
    int createWithPointer(const size_t length,
                          const OCLBufferMode& mode,
                          SCALAR* ptr) {
        const size_t size = length * sizeof(SCALAR);
        return create(size, &mode, false, ptr, false);
    }

    template <typename SCALAR>
    int createWithPointer(const size_t length,
                          const OCLBufferMode& mode,
                          const SCALAR* ptr) {
        const size_t size = length * sizeof(SCALAR);
        return create(size, &mode, false, const_cast<SCALAR*>(ptr), false);
    }

    template <typename SCALAR>
    int createAllocate(const size_t length,
                       const OCLBufferMode& mode) {
        const size_t size = length * sizeof(SCALAR);
        for (size_t i = 0; i < _refcnt.size(); i++)
            if (0 == _refcnt[i]
                && size == _size[i]
                && mode.flags() == _access[i]->flags())
                return i;
        return create(size, &mode, false, MEMORY_ALIGNMENT);
    }

    template <typename SCALAR>
    int createAllocate(const size_t length,
                       const OCLBufferMode& mode,
                       const bool pinned) {
        const size_t size = length * sizeof(SCALAR);
        for (size_t i = 0; i < _refcnt.size(); i++)
            if (0 == _refcnt[i]
                && size == _size[i]
                && mode.flags() == _access[i]->flags()
                && pinned == _pinned[i])
                return i;
        return create(size, &mode, pinned, MEMORY_ALIGNMENT);
    }

    template <typename SCALAR>
    bool enqueueWrite(const size_t index,
                      const size_t offset,
                      const size_t length) {
        cl_event event;
        if (CL_SUCCESS != clEnqueueWriteBuffer(_queue,
                                               _handles[index],
                                               CL_FALSE, // non-blocking
                                               offset * sizeof(SCALAR),
                                               length * sizeof(SCALAR),
                                               _ptrs[index],
                                               0,
                                               NULL,
                                               &event))
            return false;
        _writeEvents.push_back(event);
        return true;
    }

    template <typename SCALAR>
    bool enqueueRead(const size_t index,
                     const size_t offset,
                     const size_t length) {
        cl_event event;
        if (CL_SUCCESS != clEnqueueReadBuffer(_queue,
                                              _handles[index],
                                              CL_FALSE, // non-blocking
                                              offset * sizeof(SCALAR),
                                              length * sizeof(SCALAR),
                                              _ptrs[index],
                                              0,
                                              NULL,
                                              &event))
            return false;
        _readEvents.push_back(event);
        return true;
    }

    bool waitWrite(void);
    bool waitRead(void);

    void checkout(const size_t index);
    void release(const size_t index);

    void* ptr(const size_t index);
    const void* ptr(const size_t index) const;

    cl_mem& handle(const size_t index);
    const cl_mem& handle(const size_t index) const;
};

////////////////////////////////////////
// heap of images

class OCLHeapOfImages
{
    friend class OCLdevice;

    const cl_context                  _context;
    const cl_command_queue            _queue;

    Type<void*>::aligned_vector       _ptrs;
    std::vector<bool>                 _freePtr;
    Type<cl_mem>::aligned_vector      _handles;
    std::vector<const OCLImageMode*>  _access;
    std::vector<bool>                 _pinned;
    std::vector<size_t>               _width;
    std::vector<size_t>               _height;
    std::vector<size_t>               _refcnt;

    Type<cl_event>::aligned_vector    _writeEvents;
    Type<cl_event>::aligned_vector    _readEvents;

    int create(const size_t width,  // texel dimensions (x4 or x2 for matrix)
               const size_t height, // texel dimensions
               const OCLImageMode* mode,
               const bool pinned,
               void* ptr,
               const bool freePtr);

    int create(const size_t width,  // texel dimensions (x4 or x2 for matrix)
               const size_t height, // texel dimensions
               const OCLImageMode* mode,
               const bool pinned,
               const size_t alignment);

    OCLHeapOfImages(const cl_context context,
                    const cl_command_queue queue);

public:
    ~OCLHeapOfImages(void);

    size_t scavenge(void);

    int createWithPointer(const size_t width,  // texel dimensions
                                               //     (x4 or x2 for matrix)
                          const size_t height, // texel dimensions
                          const OCLImageMode& mode,
                          void* ptr);

    int createWithPointer(const size_t width,  // texel dimensions
                                               //     (x4 or x2 for matrix)
                          const size_t height, // texel dimensions
                          const OCLImageMode& mode,
                          const void* ptr);

    template <typename SCALAR>
    int createAllocate(const size_t width,  // texel dimensions
                                            //     (x4 or x2 for matrix)
                       const size_t height, // texel dimensions
                       const OCLImageMode& mode) {
        for (size_t i = 0; i < _refcnt.size(); i++)
            if (0 == _refcnt[i]
                && width = _width[i]
                && height = _height[i]
                && mode.flags() == _access[i]->flags())
                return i;
        return create(width,
                      height,
                      &mode,
                      false,
                      imgveclen<SCALAR>() * MEMORY_ALIGNMENT);
    }

    template <typename SCALAR>
    int createAllocate(const size_t width,  // texel dimensions
                                            //     (x4 or x2 for matrix)
                       const size_t height, // texel dimensions
                       const OCLImageMode& mode,
                       const bool pinned) {
        for (size_t i = 0; i < _refcnt.size(); i++)
            if (0 == _refcnt[i]
                && width = _width[i]
                && height = _height[i]
                && mode.flags() == _access[i]->flags()
                && pinned == _pinned[i])
                return i;
        return create(width,
                      height,
                      &mode,
                      pinned,
                      imgveclen<SCALAR>() * MEMORY_ALIGNMENT);
    }

    template <typename SCALAR>
    bool enqueueWrite(const size_t index,
                      const size_t originWidth,
                      const size_t originHeight,
                      const size_t regionWidth,
                      const size_t regionHeight) {
        size_t origin[3], region[3];
        origin[0] = originWidth;
        origin[1] = originHeight;
        origin[2] = 0;
        region[0] = regionWidth;
        region[1] = regionHeight;
        region[2] = 0;
        cl_event event;
        if (CL_SUCCESS != clEnqueueWriteImage(_queue,
                                              _handles[index],
                                              CL_FALSE, // non-blocking
                                              origin,
                                              region,
                                              0,
                                              0,
                                              _ptrs[index],
                                              0,
                                              NULL,
                                              &event))
            return false;
        _writeEvents.push_back(event);
        return true;
    }

    template <typename SCALAR>
    bool enqueueRead(const size_t index,
                     const size_t originWidth,
                     const size_t originHeight,
                     const size_t regionWidth,
                     const size_t regionHeight) {
        size_t origin[3], region[3];
        origin[0] = originWidth;
        origin[1] = originHeight;
        origin[2] = 0;
        region[0] = regionWidth;
        region[1] = regionHeight;
        region[2] = 0;
        cl_event event;
        if (CL_SUCCESS != clEnqueueReadImage(_queue,
                                             _handles[index],
                                             CL_FALSE, // non-blocking
                                             origin,
                                             region,
                                             0,
                                             0,
                                             _ptrs[index],
                                             0,
                                             NULL,
                                             &event))
            return false;
        _readEvents.push_back(event);
        return true;
    }

    bool waitWrite(void);
    bool waitRead(void);

    void checkout(const size_t index);
    void release(const size_t index);

    void* ptr(const size_t index);
    const void* ptr(const size_t index) const;

    cl_mem& handle(const size_t index);
    const cl_mem& handle(const size_t index) const;
};

////////////////////////////////////////
// heap of kernels

class OCLHeapOfKernels
{
    friend class OCLdevice;

    const cl_device_id                      _device;
    const cl_context                        _context;
    const cl_command_queue                  _queue;

    Type<cl_program>::aligned_vector        _programs;
    Type<cl_kernel>::aligned_vector         _kernels;
    std::vector< std::vector<std::string> > _source;
    std::vector< std::string >              _kernelName;
    std::vector<size_t>                     _refcnt;

    Type<cl_event>::aligned_vector          _events;

    int create(const std::vector<std::string>& source,
               const std::string& options,
               const std::string& kernelName);

    OCLHeapOfKernels(const cl_device_id device,
                     const cl_context context,
                     const cl_command_queue queue);

public:
    ~OCLHeapOfKernels(void);

    size_t scavenge(void);

    int create(const std::string& source,
               const std::string& kernelName);

    int create(const std::vector<std::string>& source,
               const std::string& kernelName);

    bool enqueue(const size_t index,
                 const std::vector<size_t>& global,
                 const std::vector<size_t>& local);

    bool wait(void);

    // global memory or image
    bool setArg(const size_t index,
                const size_t argIndex,
                const cl_mem object);

    // local memory
    template <typename SCALAR>
    bool setArgLocal(const size_t index,
                     const size_t argIndex,
                     const size_t length) {
        return CL_SUCCESS == clSetKernelArg(_kernels[index],
                                            argIndex,
                                            length * sizeof(SCALAR),
                                            NULL);
    }

    template <typename SCALAR>
    bool setArgValue(const size_t index,
                     const size_t argIndex,
                     const SCALAR value) {
        return CL_SUCCESS == clSetKernelArg(_kernels[index],
                                            argIndex,
                                            sizeof(SCALAR),
                                            &value);
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

    OCLHeapOfMemoryBuffers _bufHeap;
    OCLHeapOfImages        _imgHeap;
    OCLHeapOfKernels       _krnlHeap;

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

    void scavenge(void);
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
              const OCLBufferMode& accessMode)
        : _heap(cdev.buffers()),
          _index(_heap.createAllocate<SCALAR>(length, accessMode)),
          _length(length)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    OCLmembuf(OCLdevice& cdev,
              const size_t length,
              const OCLBufferMode& accessMode,
              const bool pinned)
        : _heap(cdev.buffers()),
          _index(_heap.createAllocate<SCALAR>(length, accessMode, pinned)),
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
              const OCLImageMode& accessMode)
        : _heap(cdev.images()),
          _index(_heap.createAllocate<SCALAR>(width / imgveclen<SCALAR>(),
                                              height,
                                              accessMode)),
          _width(width),
          _height(height)
    {
        if (-1 != _index) _heap.checkout(_index);
    }

    OCLimgbuf(OCLdevice& cdev,
           const size_t width,  // matrix dimensions
           const size_t height, // matrix dimensions
           const OCLImageMode& accessMode,
           const bool pinned)
        : _heap(cdev.images()),
          _index(_heap.createAllocate<SCALAR>(width / imgveclen<SCALAR>(),
                                              height,
                                              accessMode,
                                              pinned)),
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

public:
    OCLkernel(OCLdevice& cdev);
    OCLkernel(OCLdevice& cdev,
              const std::string& kernelName,
              const std::string& source);
    OCLkernel(OCLdevice& cdev,
              const std::string& kernelName,
              const std::vector<std::string>& source);
    ~OCLkernel(void);

    void build(const std::string& kernelName,
               const std::string& source);
    void build(const std::string& kernelName,
               const std::vector<std::string>& source);

    size_t index(void) const;

    const std::vector<size_t>& global(void) const;
    const std::vector<size_t>& local(void) const;
    void workDim(const size_t globalDim, const size_t localDim);
    void clearDim(void);

    void clearArgIndex(void);

    template <typename SCALAR>
    bool setArg(const OCLmembuf<SCALAR>& mb, const bool lazy = false) {
        return _heap.setArg(_index, _argIndex++, mb.handle());
    }

    template <typename SCALAR>
    bool setArg(const OCLimgbuf<SCALAR>& ib) {
        return _heap.setArg(_index, _argIndex++, ib.handle());
    }

    template <typename SCALAR>
    bool setArgLocal(const size_t length) {
        return _heap.setArgLocal<SCALAR>(_index, _argIndex++, length);
    }

    template <typename SCALAR>
    bool setArgValue(const SCALAR value) {
        return _heap.setArgValue<SCALAR>(_index, _argIndex++, value);
    }
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
OCLdevice& operator << (OCLdevice& cdev, OCLmembuf<SCALAR>& mb) {
    if (!cdev.buffers().enqueueWrite<SCALAR>(mb.index(), 0, mb.length()))
        std::cerr << "enqueue write buffer failed" << std::endl;
    return cdev;
}

template <typename SCALAR>
OCLdevice& operator >> (OCLdevice& cdev, OCLmembuf<SCALAR>& mb) {
    if (!cdev.buffers().enqueueRead<SCALAR>(mb.index(), 0, mb.length()))
        std::cerr << "enqueue read buffer failed" << std::endl;
    return cdev;
}

template <typename SCALAR>
OCLdevice& operator << (OCLdevice& cdev, OCLimgbuf<SCALAR>& ib) {
    if (!cdev.images().enqueueWrite<SCALAR>(ib.index(),
                                            0,
                                            0,
                                            ib.width() / imgveclen<SCALAR>(),
                                            ib.height()))
        std::cerr << "enqueue write image failed" << std::endl;
    return cdev;
}

template <typename SCALAR>
OCLdevice& operator >> (OCLdevice& cdev, OCLimgbuf<SCALAR>& ib) {
    if (!cdev.images().enqueueRead<SCALAR>(ib.index(),
                                           0,
                                           0,
                                           ib.width() / imgveclen<SCALAR>(),
                                           ib.height()))
        std::cerr << "enqueue read image failed" << std::endl;
    return cdev;
}

// kernel execution ////////////////////

OCLdevice& operator << (OCLdevice& cdev, OCLkernel& krnl);

class OCLWorkIndex {
    const size_t _global;
    const size_t _local;
public:
    OCLWorkIndex(const size_t global, const size_t local);
    size_t global(void) const;
    size_t local(void) const;
};

OCLkernel& operator << (OCLkernel& krnl, const OCLWorkIndex& dims);

// set kernel arguments ////////////////

template <typename SCALAR>
OCLkernel& operator << (OCLkernel& krnl, OCLmembuf<SCALAR>& mb) {
    if (!krnl.setArg(mb))
        std::cerr << "set memory buffer argument failed" << std::endl;
    return krnl;
}

template <typename SCALAR>
OCLkernel& operator << (OCLkernel& krnl, OCLimgbuf<SCALAR>& ib) {
    if (!krnl.setArg(ib))
        std::cerr << "set image argument failed" << std::endl;
    return krnl;
}

template <typename SCALAR>
class OCLLocalbuf {
    const size_t _length;
public:
    OCLLocalbuf(const size_t length) : _length(length) { }
    size_t length(void) const { return _length; }
};

template <typename SCALAR>
OCLkernel& operator << (OCLkernel& krnl, OCLLocalbuf<SCALAR>& lb) {
    if (!krnl.setArgLocal<SCALAR>(lb.length()))
        std::cerr << "set local memory argument failed" << std::endl;
    return krnl;
}

template <typename SCALAR>
OCLkernel& operator << (OCLkernel& krnl, const SCALAR value) {
    if (!krnl.setArgValue<SCALAR>(value))
        std::cerr << "set argument value failed" << std::endl;
    return krnl;
}

}; // namespace chai_internal

#endif
