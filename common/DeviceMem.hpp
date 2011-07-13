// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_DEVICE_MEM_HPP_
#define _CHAI_DEVICE_MEM_HPP_

#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// multiple types of compute device
// memory

struct DeviceMem
{
    virtual ~DeviceMem(void);

    virtual bool isOk(void) const = 0;

    virtual void insert(OCLkernel&) = 0;
    virtual void insert(OCLdevice&) = 0;
    virtual void extract(OCLdevice&) = 0;
};

////////////////////////////////////////
// single precision memory buffer

class DeviceMembuf32 : public DeviceMem
{
    OCLmembuf<float> _membuf;

public:
    DeviceMembuf32(OCLdevice& cdev,
                   const size_t length);

    DeviceMembuf32(OCLdevice& cdev,
                   const size_t length,
                   float* ptr);

    ~DeviceMembuf32(void);

    bool isOk(void) const;
    float* ptrMem(void);

    void insert(OCLkernel&);
    void insert(OCLdevice&);
    void extract(OCLdevice&);
};

////////////////////////////////////////
// double precision memory buffer

class DeviceMembuf64 : public DeviceMem
{
    OCLmembuf<double> _membuf;

public:
    DeviceMembuf64(OCLdevice& cdev,
                   const size_t length);

    DeviceMembuf64(OCLdevice& cdev,
                   const size_t length,
                   double* ptr);

    ~DeviceMembuf64(void);

    bool isOk(void) const;
    double* ptrMem(void);

    void insert(OCLkernel&);
    void insert(OCLdevice&);
    void extract(OCLdevice&);
};

////////////////////////////////////////
// single precision image texture

class DeviceImgbuf32 : public DeviceMem
{
    OCLimgbuf<float> _imgbuf;

public:
    DeviceImgbuf32(OCLdevice&,
                   const size_t width,
                   const size_t height,
                   float* ptr);

    DeviceImgbuf32(OCLdevice&,
                   const size_t width,
                   const size_t height,
                   const float* ptr);

    ~DeviceImgbuf32(void);

    bool isOk(void) const;

    void insert(OCLkernel&);
    void insert(OCLdevice&);
    void extract(OCLdevice&);
};

////////////////////////////////////////
// double precision image texture

class DeviceImgbuf64 : public DeviceMem
{
    OCLimgbuf<double> _imgbuf;

public:
    DeviceImgbuf64(OCLdevice&,
                   const size_t width,
                   const size_t height,
                   double* ptr);

    DeviceImgbuf64(OCLdevice&,
                   const size_t width,
                   const size_t height,
                   const double* ptr);

    ~DeviceImgbuf64(void);

    bool isOk(void) const;

    void insert(OCLkernel&);
    void insert(OCLdevice&);
    void extract(OCLdevice&);
};

}; // namespace chai_internal

#endif
