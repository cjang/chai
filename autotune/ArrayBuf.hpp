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

    const size_t _prec;
    const size_t _vecLen;

    const size_t _W;
    const size_t _H;
    const size_t _slots;

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
                 const size_t PREC,
                 const size_t vecLen,
                 const size_t W,
                 const size_t H,
                 const size_t slots,
                 const bool hostarray);

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t vecLen,
                 const size_t W,
                 const size_t H,
                 const size_t slots,
                 float* ptr,
                 const bool hostarray);

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t vecLen,
                 const size_t W,
                 const size_t H,
                 const size_t slots,
                 double* ptr,
                 const bool hostarray);

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t vecLen,
                 const size_t W,
                 const size_t H,
                 const size_t slots,
                 int32_t* ptr,
                 const bool hostarray);

    void initBuf(OCLdevice& cdev,
                 const int mode,
                 const size_t vecLen,
                 const size_t W,
                 const size_t H,
                 const size_t slots,
                 uint32_t* ptr,
                 const bool hostarray);

public:
    enum AccessMode { READ = 0, WRITE = 1, READWRITE = 2 };

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,  // READ, WRITE, READWRITE
             const size_t PREC,      // 0, 1, sizeof(float), sizeof(double)
             const size_t vecLen,    // 0 is image, 1|2|4 is memory buffer
             const size_t W,
             const size_t H,
             const size_t slots,
             const bool hostarray = false);

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,  // READ, WRITE, READWRITE
             const size_t vecLen,    // 0 is image, 1|2|4 is memory buffer
             const size_t W,
             const size_t H,
             const size_t slots,
             float* ptr,
             const bool hostarray = false);

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,  // READ, WRITE, READWRITE
             const size_t vecLen,    // 0 is image, 1|2|4 is memory buffer
             const size_t W,
             const size_t H,
             const size_t slots,
             double* ptr,
             const bool hostarray = false);

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,  // READ, WRITE, READWRITE
             const size_t vecLen,    // 0 is image, 1|2|4 is memory buffer
             const size_t W,
             const size_t H,
             const size_t slots,
             int32_t* ptr,
             const bool hostarray = false);

    ArrayBuf(OCLdevice& cdev,
             const AccessMode mode,  // READ, WRITE, READWRITE
             const size_t vecLen,    // 0 is image, 1|2|4 is memory buffer
             const size_t W,
             const size_t H,
             const size_t slots,
             uint32_t* ptr,
             const bool hostarray = false);

    ~ArrayBuf(void);

    size_t vecLength(void) const;
    size_t prec(void) const;

    size_t W(void) const;
    size_t H(void) const;
    size_t slots(void) const;

    bool bufOk(void) const;
    bool isImgbuf(void) const;
    bool isMembuf(void) const;

    double getArrayElem(const size_t absoluteRow,
                        const size_t col) const;

    double getArrayElem(const size_t slotIndex,
                        const size_t row,
                        const size_t col) const;

    double getHostArrayElem(const size_t absoluteRow,
                            const size_t col) const;

    double getHostArrayElem(const size_t slotIndex,
                            const size_t row,
                            const size_t col) const;

    void setArrayElem(const size_t absoluteRow,
                      const size_t col,
                      const double value);

    void setArrayElem(const size_t slotIndex,
                      const size_t row,
                      const size_t col,
                      const double value);

    void setHostArrayElem(const size_t absoluteRow,
                          const size_t col,
                          const double value);

    void setHostArrayElem(const size_t slotIndex,
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
