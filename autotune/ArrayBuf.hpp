// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_ARRAY_BUF_HPP_
#define _CHAI_ARRAY_BUF_HPP_

#include <map>

#include "OCLdevice.hpp"

namespace chai_internal {

////////////////////////////////////////
// OCLmembuf or OCLimgbuf

class ArrayBuf
{
    OCLdevice&   _cdev;

    const size_t _packing;
    const size_t _width;
    const size_t _height;
    const size_t _precision;
    const size_t _vectorLength;

    OCLmembuf< float >*  _membuf32;
    OCLmembuf< double >* _membuf64;
    OCLimgbuf< float >*  _imgbuf32;
    OCLimgbuf< double >* _imgbuf64;

    float*  _hostarray32;
    double* _hostarray64;

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

public:
    enum AccessMode { READ = 0, WRITE = 1, READWRITE = 2 };

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,      // READ, WRITE, READWRITE
             const size_t packing,
             const size_t width,
             const size_t height,
             const size_t precision,     // sizeof(float), sizeof(double)
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

    ~ArrayBuf(void);

    size_t packing(void) const;
    size_t width(void) const;
    size_t height(void) const;
    size_t precision(void) const;
    size_t vectorLength(void) const;

    bool bufOk(void) const;
    bool isImgbuf(void) const;
    bool isMembuf(void) const;
    bool isSinglePrecision(void) const;
    bool isDoublePrecision(void) const;

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

    void fillValue(const float val32);
    void fillValue(const double val64);
    void fillZeros(void);
    void fillOnes(void);
    void fillRandom(void);

    bool flushToDevice(void);
    bool flushFromDevice(void);

    bool argumentToKernel(OCLkernel& ckernel);
};

}; // namespace chai_internal

#endif
