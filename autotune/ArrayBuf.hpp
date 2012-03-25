// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_ARRAY_BUF_HPP_
#define _CHAI_ARRAY_BUF_HPP_

#include <map>
#include <stdint.h>

#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// OCLmembuf or OCLimgbuf

class ArrayBuf
{
    OCLdevice& _cdev;

    const size_t _packing;
    const size_t _width;
    const size_t _height;
    const size_t _precision;
    const size_t _vectorLength;

    enum BufferSwitch { UNSET, MBF, MBD, MBI, MBU, IBF, IBD, IBI, IBU };
    BufferSwitch _devbuf;

    OCLmembuf< float >*    _membufFloat;
    OCLmembuf< double >*   _membufDouble;
    OCLmembuf< int32_t >*  _membufInt32;
    OCLmembuf< uint32_t >* _membufUInt32;

    OCLimgbuf< float >*    _imgbufFloat;
    OCLimgbuf< double >*   _imgbufDouble;
    OCLimgbuf< int32_t >*  _imgbufInt32;
    OCLimgbuf< uint32_t >* _imgbufUInt32;

    float*    _hostarrayFloat;
    double*   _hostarrayDouble;
    int32_t*  _hostarrayInt32;
    uint32_t* _hostarrayUInt32;

    const OCLImageMode& imgbufMode(const int) const;
    const OCLBufferMode& membufMode(const int) const;

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t packing,
                 const size_t width,
                 const size_t height,
                 const size_t precision,
                 const size_t vectorLength,
                 const bool hostarray);

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t packing,
                 const size_t width,
                 const size_t height,
                 float* ptr,
                 const size_t vectorLength,
                 const bool hostarray);

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t packing,
                 const size_t width,
                 const size_t height,
                 double* ptr,
                 const size_t vectorLength,
                 const bool hostarray);

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t packing,
                 const size_t width,
                 const size_t height,
                 int32_t* ptr,
                 const size_t vectorLength,
                 const bool hostarray);

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t packing,
                 const size_t width,
                 const size_t height,
                 uint32_t* ptr,
                 const size_t vectorLength,
                 const bool hostarray);

public:
    enum AccessMode { READ = 0, WRITE = 1, READWRITE = 2 };

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,      // READ, WRITE, READWRITE
             const size_t packing,
             const size_t width,
             const size_t height,
             const size_t precision,     // 0, 1, sizeof(float), sizeof(double)
             const size_t vectorLength,  // 0 is image, 1|2|4 is memory buffer
             const bool hostarray = false);

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,      // READ, WRITE, READWRITE
             const size_t packing,
             const size_t width,
             const size_t height,
             float* ptr,
             const size_t vectorLength,  // 0 is image, 1|2|4 is memory buffer
             const bool hostarray = false);

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,      // READ, WRITE, READWRITE
             const size_t packing,
             const size_t width,
             const size_t height,
             double* ptr,
             const size_t vectorLength,  // 0 is image, 1|2|4 is memory buffer
             const bool hostarray = false);

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,      // READ, WRITE, READWRITE
             const size_t packing,
             const size_t width,
             const size_t height,
             int32_t* ptr,
             const size_t vectorLength,  // 0 is image, 1|2|4 is memory buffer
             const bool hostarray = false);

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,      // READ, WRITE, READWRITE
             const size_t packing,
             const size_t width,
             const size_t height,
             uint32_t* ptr,
             const size_t vectorLength,  // 0 is image, 1|2|4 is memory buffer
             const bool hostarray = false);

    ~ArrayBuf(void);

    size_t packing(void) const;
    size_t width(void) const;
    size_t height(void) const;
    const size_t precision(void) const;
    size_t vectorLength(void) const;

    bool bufOk(void) const;
    bool isImgbuf(void) const;
    bool isMembuf(void) const;

    double getArrayElem(const size_t absoluteRow,
                        const size_t col) const;

    double getArrayElem(const size_t packing,
                        const size_t row,
                        const size_t col) const;

    double getHostArrayElem(const size_t absoluteRow,
                            const size_t col) const;

    double getHostArrayElem(const size_t packing,
                            const size_t row,
                            const size_t col) const;

    void setArrayElem(const size_t absoluteRow,
                      const size_t col,
                      const double value);

    void setArrayElem(const size_t packing,
                      const size_t row,
                      const size_t col,
                      const double value);

    void setHostArrayElem(const size_t row,
                          const size_t col,
                          const double);

    void setHostArrayElem(const size_t packing,
                          const size_t row,
                          const size_t col,
                          const double value);

    std::map<double, size_t> arrayHistogram(void) const;
    double absdiffHostArray(void) const;
    std::map< int, size_t > absdiffHistogram(void) const;

    void fillValue(const float);
    void fillValue(const double);
    void fillValue(const int32_t);
    void fillValue(const uint32_t);
    void fillZeros(void);
    void fillOnes(void);
    void fillRandom(void);

    bool flushToDevice(void);
    bool flushFromDevice(void);

    bool argumentToKernel(OCLkernel& ckernel);
};

}; // namespace chai_internal

#endif
