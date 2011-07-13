// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "DeviceMem.hpp"

namespace chai_internal {

////////////////////////////////////////
// multiple types of compute device
// memory

DeviceMem::~DeviceMem(void) { }

////////////////////////////////////////
// single precision memory buffer

DeviceMembuf32::DeviceMembuf32(OCLdevice& cdev,
                               const size_t length)
    : _membuf(OCLmembuf<float>(cdev, length, READWRITE)) { }

DeviceMembuf32::DeviceMembuf32(OCLdevice& cdev,
                               const size_t length,
                               float* ptr)
    : _membuf(OCLmembuf<float>(cdev, length, ptr)) { }

DeviceMembuf32::~DeviceMembuf32(void) { }

bool DeviceMembuf32::isOk(void) const
{
    return _membuf.isOk();
}

float* DeviceMembuf32::ptrMem(void)
{
    return static_cast< float* >( _membuf );
}

void DeviceMembuf32::insert(OCLkernel& krnl)
{
    krnl << _membuf;
}

void DeviceMembuf32::insert(OCLdevice& dev)
{
    dev << _membuf;
}

void DeviceMembuf32::extract(OCLdevice& dev)
{
    dev >> _membuf;
}

////////////////////////////////////////
// double precision memory buffer

DeviceMembuf64::DeviceMembuf64(OCLdevice& cdev,
                               const size_t length)
    : _membuf(OCLmembuf<double>(cdev, length, READWRITE)) { }

DeviceMembuf64::DeviceMembuf64(OCLdevice& cdev,
                               const size_t length,
                               double* ptr)
    : _membuf(OCLmembuf<double>(cdev, length, ptr)) { }

DeviceMembuf64::~DeviceMembuf64(void) { }

bool DeviceMembuf64::isOk(void) const
{
    return _membuf.isOk();
}

double* DeviceMembuf64::ptrMem(void)
{
    return static_cast< double* >( _membuf );
}

void DeviceMembuf64::insert(OCLkernel& krnl)
{
    krnl << _membuf;
}

void DeviceMembuf64::insert(OCLdevice& dev)
{
    dev << _membuf;
}

void DeviceMembuf64::extract(OCLdevice& dev)
{
    dev >> _membuf;
}

////////////////////////////////////////
// single precision image texture

DeviceImgbuf32::DeviceImgbuf32(OCLdevice& cdev,
                               const size_t width,
                               const size_t height,
                               float* ptr)
    : _imgbuf(OCLimgbuf<float>(cdev, width, height, ptr)) { }

DeviceImgbuf32::DeviceImgbuf32(OCLdevice& cdev,
                               const size_t width,
                               const size_t height,
                               const float* ptr)
    : _imgbuf(OCLimgbuf<float>(cdev, width, height, ptr)) { }

DeviceImgbuf32::~DeviceImgbuf32(void) { }

bool DeviceImgbuf32::isOk(void) const
{
    return _imgbuf.isOk();
}

void DeviceImgbuf32::insert(OCLkernel& krnl)
{
    krnl << _imgbuf;
}

void DeviceImgbuf32::insert(OCLdevice& dev)
{
    dev << _imgbuf;
}

void DeviceImgbuf32::extract(OCLdevice& dev)
{
    dev >> _imgbuf;
}

////////////////////////////////////////
// double precision image texture

DeviceImgbuf64::DeviceImgbuf64(OCLdevice& cdev,
                               const size_t width,
                               const size_t height,
                               double* ptr)
    : _imgbuf(OCLimgbuf<double>(cdev, width, height, ptr)) { }

DeviceImgbuf64::DeviceImgbuf64(OCLdevice& cdev,
                               const size_t width,
                               const size_t height,
                               const double* ptr)
    : _imgbuf(OCLimgbuf<double>(cdev, width, height, ptr)) { }

DeviceImgbuf64::~DeviceImgbuf64(void) { }

bool DeviceImgbuf64::isOk(void) const
{
    return _imgbuf.isOk();
}

void DeviceImgbuf64::insert(OCLkernel& krnl)
{
    krnl << _imgbuf;
}

void DeviceImgbuf64::insert(OCLdevice& dev)
{
    dev << _imgbuf;
}

void DeviceImgbuf64::extract(OCLdevice& dev)
{
    dev >> _imgbuf;
}

}; // namespace chai_internal
