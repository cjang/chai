// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>
#include <string.h>

#include "ArrayBuf.hpp"
#include "ArrayBufUtil.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// OCLmembuf or OCLimgbuf

const OCLImageMode& ArrayBuf::imgbufMode(const int mode) const
{
    switch (mode)
    {
        case (READ) :
            return chai_internal::READ;
            break;

        case (WRITE) :
        case (READWRITE) :
            return chai_internal::WRITE;
            break;
    }
}

const OCLBufferMode& ArrayBuf::membufMode(const int mode) const
{
    switch (mode)
    {
        case (READ) :
            return chai_internal::READ;
            break;

        case (WRITE) :
            return chai_internal::WRITE;
            break;

        case (READWRITE) :
            return chai_internal::READWRITE;
            break;
    }
}

void ArrayBuf::initBuf(OCLdevice& cdev,
                       const int mode,
                       const size_t packing,
                       const size_t width,
                       const size_t height,
                       const size_t precision,
                       const size_t vectorLength,
                       const bool hostarray)
{
    const bool pinned = false;

    switch (precision)
    {
        case (sizeof(float)) :

            switch (vectorLength)
            {
                case (0) :

                    _imgbuf32 = new OCLimgbuf<float>(
                                        cdev,
                                        width,
                                        packing * height,
                                        imgbufMode(mode),
                                        pinned);

                    if (! _imgbuf32->isOk())
                    {
                        delete _imgbuf32;
                        _imgbuf32 = NULL;
                    }

                    if (hostarray)
                        _hostarray32 = new float[packing * width * height];

                    break;

                case (1) :
                case (2) :
                case (4) :

                    _membuf32 = new OCLmembuf<float>(
                                        cdev,
                                        packing * width * height,
                                        membufMode(mode),
                                        pinned,
                                        vectorLength);

                    if (! _membuf32->isOk())
                    {
                        delete _membuf32;
                        _membuf32 = NULL;
                    }

                    if (hostarray)
                        _hostarray32 = new float[packing * width * height];

                    break;
            }
            break;

        case (sizeof(double)) :

            switch (vectorLength)
            {
                case (0) :

                    _imgbuf64 = new OCLimgbuf<double>(
                                        cdev,
                                        width,
                                        packing * height,
                                        imgbufMode(mode),
                                        pinned);

                    if (! _imgbuf64->isOk())
                    {
                        delete _imgbuf64;
                        _imgbuf64 = NULL;
                    }

                    if (hostarray)
                        _hostarray64 = new double[packing * width * height];

                    break;

                case (1) :
                case (2) :
                case (4) :

                    _membuf64 = new OCLmembuf<double>(
                                        cdev,
                                        packing * width * height,
                                        membufMode(mode),
                                        pinned,
                                        vectorLength);

                    if (! _membuf64->isOk())
                    {
                        delete _membuf64;
                        _membuf64 = NULL;
                    }

                    if (hostarray)
                        _hostarray64 = new double[packing * width * height];

                    break;
            }
            break;
    }
}

void ArrayBuf::initBuf(OCLdevice& cdev,
                       const int mode,
                       const size_t packing,
                       const size_t width,
                       const size_t height,
                       float* ptr,
                       const size_t vectorLength,
                       const bool hostarray)
{
    switch (vectorLength)
    {
        case (0) :

            if (READ == mode)
            {
                _imgbuf32 = new OCLimgbuf<float>(
                                    cdev,
                                    width,
                                    packing * height,
                                    static_cast<const float*>(ptr) );
            }
            else if (WRITE == mode)
            {
                _imgbuf32 = new OCLimgbuf<float>(
                                    cdev,
                                    width,
                                    packing * height,
                                    ptr );
            }

            if (_imgbuf32 && ! _imgbuf32->isOk())
            {
                delete _imgbuf32;
                _imgbuf32 = NULL;
            }

            if (hostarray)
                _hostarray32 = new float[packing * width * height];

            break;

        case (1) :
        case (2) :
        case (4) :

            if (READ == mode)
            {
                _membuf32 = new OCLmembuf<float>(
                                    cdev,
                                    packing * width * height,
                                    static_cast<const float*>(ptr) );
            }
            else
            {
                _membuf32 = new OCLmembuf<float>(
                                    cdev,
                                    packing * width * height,
                                    ptr );
            }

            if (! _membuf32->isOk())
            {
                delete _membuf32;
                _membuf32 = NULL;
            }

            if (hostarray)
                _hostarray32 = new float[packing * width * height];

            break;
    }
}

void ArrayBuf::initBuf(OCLdevice& cdev,
                       const int mode,
                       const size_t packing,
                       const size_t width,
                       const size_t height,
                       double* ptr,
                       const size_t vectorLength,
                       const bool hostarray)
{
    switch (vectorLength)
    {
        case (0) :

            if (READ == mode)
            {
                _imgbuf64 = new OCLimgbuf<double>(
                                    cdev,
                                    width,
                                    packing * height,
                                    static_cast<const double*>(ptr) );
            }
            else if (WRITE == mode)
            {
                _imgbuf64 = new OCLimgbuf<double>(
                                    cdev,
                                    width,
                                    packing * height,
                                    ptr );
            }

            if (_imgbuf64 && ! _imgbuf64->isOk())
            {
                delete _imgbuf64;
                _imgbuf64 = NULL;
            }

            if (hostarray)
                _hostarray64 = new double[packing * width * height];

            break;

        case (1) :
        case (2) :
        case (4) :

            if (READ == mode)
            {
                _membuf64 = new OCLmembuf<double>(
                                    cdev,
                                    packing * width * height,
                                    static_cast<const double*>(ptr) );
            }
            else
            {
                _membuf64 = new OCLmembuf<double>(
                                    cdev,
                                    packing * width * height,
                                    ptr );
            }

            if (! _membuf64->isOk())
            {
                delete _membuf64;
                _membuf64 = NULL;
            }

            if (hostarray)
                _hostarray64 = new double[packing * width * height];

            break;
    }
}

ArrayBuf::ArrayBuf(OCLdevice& cdev,
                   const AccessMode mode,
                   const size_t packing,
                   const size_t width,
                   const size_t height,
                   const size_t precision,
                   const size_t vectorLength,
                   const bool hostarray)
    : _cdev(cdev),
      _packing(packing),
      _width(width),
      _height(height),
      _precision(precision),
      _vectorLength(vectorLength),
      _membuf32(NULL),
      _membuf64(NULL),
      _imgbuf32(NULL),
      _imgbuf64(NULL),
      _hostarray32(NULL),
      _hostarray64(NULL)
{
    initBuf(cdev,
            mode,
            packing,
            width,
            height,
            precision,
            vectorLength,
            hostarray);
}

ArrayBuf::ArrayBuf(OCLdevice& cdev,
                   const AccessMode mode,
                   const size_t packing,
                   const size_t width,
                   const size_t height,
                   float* ptr,
                   const size_t vectorLength,
                   const bool hostarray)
    : _cdev(cdev),
      _packing(packing),
      _width(width),
      _height(height),
      _precision(sizeof(float)),
      _vectorLength(vectorLength),
      _membuf32(NULL),
      _membuf64(NULL),
      _imgbuf32(NULL),
      _imgbuf64(NULL),
      _hostarray32(NULL),
      _hostarray64(NULL)
{
    initBuf(cdev,
            mode,
            packing,
            width,
            height,
            ptr,
            vectorLength,
            hostarray);
}

ArrayBuf::ArrayBuf(OCLdevice& cdev,
                   const AccessMode mode,
                   const size_t packing,
                   const size_t width,
                   const size_t height,
                   double* ptr,
                   const size_t vectorLength,
                   const bool hostarray)
    : _cdev(cdev),
      _packing(packing),
      _width(width),
      _height(height),
      _precision(sizeof(double)),
      _vectorLength(vectorLength),
      _membuf32(NULL),
      _membuf64(NULL),
      _imgbuf32(NULL),
      _imgbuf64(NULL),
      _hostarray32(NULL),
      _hostarray64(NULL)
{
    initBuf(cdev,
            mode,
            packing,
            width,
            height,
            ptr,
            vectorLength,
            hostarray);
}

ArrayBuf::~ArrayBuf(void)
{
    delete _membuf32;
    delete _membuf64;
    delete _imgbuf32;
    delete _imgbuf64;
    delete[] _hostarray32;
    delete[] _hostarray64;
}

size_t ArrayBuf::packing(void) const
{
    return _packing;
}

size_t ArrayBuf::width(void) const
{
    return _width;
}

size_t ArrayBuf::height(void) const
{
    return _height;
}

size_t ArrayBuf::precision(void) const
{
    return _precision;
}

size_t ArrayBuf::vectorLength(void) const
{
    return _vectorLength;
}

bool ArrayBuf::bufOk(void) const
{
    return
        NULL != _membuf32 ||
        NULL != _membuf64 ||
        NULL != _imgbuf32 ||
        NULL != _imgbuf64;
}

bool ArrayBuf::isImgbuf(void) const
{
    return NULL != _imgbuf32 || NULL != _imgbuf64;
}

bool ArrayBuf::isMembuf(void) const
{
    return NULL != _membuf32 || NULL != _membuf64;
}

bool ArrayBuf::isSinglePrecision() const
{
    return NULL != _membuf32 || NULL != _imgbuf32;
}

bool ArrayBuf::isDoublePrecision() const
{
    return NULL != _membuf64 || NULL != _imgbuf64;
}

double ArrayBuf::getArrayElem(const size_t absoluteRow,
                              const size_t col) const
{
    const size_t idx = absoluteRow * _width + col;

    if (_membuf32)
    {
        return (*_membuf32)[idx];
    }
    else if (_membuf64)
    {
        return (*_membuf64)[idx];
    }
    else if (_imgbuf32)
    {
        return (*_imgbuf32)[idx];
    }
    else if (_imgbuf64)
    {
        return (*_imgbuf64)[idx];
    }

    return 0;
}

double ArrayBuf::getArrayElem(const size_t packing,
                              const size_t row,
                              const size_t col) const
{
    return getArrayElem(row + packing * _height, col);
}

double ArrayBuf::getHostArrayElem(const size_t absoluteRow,
                                  const size_t col) const
{
    const size_t idx = absoluteRow * _width + col;

    if (_hostarray32)
    {
        return _hostarray32[idx];
    }
    else if (_hostarray64)
    {
        return _hostarray64[idx];
    }

    return 0;
}

double ArrayBuf::getHostArrayElem(const size_t packing,
                                  const size_t row,
                                  const size_t col) const
{
    return getHostArrayElem(row + packing * _height, col);
}

void ArrayBuf::setArrayElem(const size_t absoluteRow,
                            const size_t col,
                            const double value)
{
    const size_t idx = absoluteRow * _width + col;

    if (_membuf32)
    {
        (*_membuf32)[idx] = value;
    }
    else if (_membuf64)
    {
        (*_membuf64)[idx] = value;
    }
    else if (_imgbuf32)
    {
        (*_imgbuf32)[idx] = value;
    }
    else if (_imgbuf64)
    {
        (*_imgbuf64)[idx] = value;
    }
}

void ArrayBuf::setArrayElem(const size_t packing,
                            const size_t row,
                            const size_t col,
                            const double value)
{
    setArrayElem(row + packing * _height, col, value);
}

void ArrayBuf::setHostArrayElem(const size_t absoluteRow,
                                const size_t col,
                                const double value)
{
    const size_t idx = absoluteRow * _width + col;

    if (_hostarray32)
    {
        _hostarray32[idx] = value;
    }
    else if (_hostarray64)
    {
        _hostarray64[idx] = value;
    }
}

void ArrayBuf::setHostArrayElem(const size_t packing,
                                const size_t row,
                                const size_t col,
                                const double value)
{
    setHostArrayElem(row + packing *_height, col, value);
}

map<double, size_t> ArrayBuf::arrayHistogram(void) const
{
    map<double, size_t> histMap;

    if (_membuf32)
    {
        const size_t len = _membuf32->length();
        for (size_t i = 0; i < len; i++)
        {
            const double elem = (*_membuf32)[i];
            if (! histMap.count(elem))
                histMap[elem] = 1;
            else
                histMap[elem]++;
        }
    }
    else if (_membuf64)
    {
        const size_t len = _membuf64->length();
        for (size_t i = 0; i < len; i++)
        {
            const double elem = (*_membuf64)[i];
            if (! histMap.count(elem))
                histMap[elem] = 1;
            else
                histMap[elem]++;
        }
    }
    else if (_imgbuf32)
    {
        const size_t len = _imgbuf32->width() * _imgbuf32->height();
        for (size_t i = 0; i < len; i++)
        {
            const double elem = (*_imgbuf32)[i];
            if (! histMap.count(elem))
                histMap[elem] = 1;
            else
                histMap[elem]++;
        }
    }
    else if (_imgbuf64)
    {
        const size_t len = _imgbuf64->width() * _imgbuf64->height();
        for (size_t i = 0; i < len; i++)
        {
            const double elem = (*_imgbuf64)[i];
            if (! histMap.count(elem))
                histMap[elem] = 1;
            else
                histMap[elem]++;
        }
    }

    return histMap;
}

double ArrayBuf::absdiffHostArray(void) const
{
    double accumDiff = 0;

    if (_membuf32)
    {
        const size_t len = _membuf32->length();
        for (size_t i = 0; i < len; i++)
        {
            const double elem1 = (*_membuf32)[i];
            const double elem2 = _hostarray32[i];
            accumDiff += fabs(elem1 - elem2);
        }
    }
    else if (_membuf64)
    {
        const size_t len = _membuf64->length();
        for (size_t i = 0; i < len; i++)
        {
            const double elem1 = (*_membuf64)[i];
            const double elem2 = _hostarray64[i];
            accumDiff += fabs(elem1 - elem2);
        }
    }
    else if (_imgbuf32)
    {
        const size_t len = _imgbuf32->width() * _imgbuf32->height();
        for (size_t i = 0; i < len; i++)
        {
            const double elem1 = (*_imgbuf32)[i];
            const double elem2 = _hostarray32[i];
            accumDiff += fabs(elem1 - elem2);
        }
    }
    else if (_imgbuf64)
    {
        const size_t len = _imgbuf64->width() * _imgbuf64->height();
        for (size_t i = 0; i < len; i++)
        {
            const double elem1 = (*_imgbuf64)[i];
            const double elem2 = _hostarray64[i];
            accumDiff += fabs(elem1 - elem2);
        }
    }

    return accumDiff;
}

map< int, size_t > ArrayBuf::absdiffHistogram(void) const
{
    size_t histN = 0, hist0 = 0, hist1 = 0, hist2 = 0,
           hist3 = 0, hist4 = 0, hist5 = 0, hist6 = 0,
           hist7 = 0;

    if (_membuf32)
    {
        const size_t len = _membuf32->length();
        for (size_t i = 0; i < len; i++)
        {
            const double elem1 = (*_membuf32)[i];
            const double elem2 = _hostarray32[i];
            const double d = fabs(elem1 - elem2);
                 if (d < 0.0000001) hist7++;
            else if (d < 0.000001) hist6++;
            else if (d < 0.00001) hist5++;
            else if (d < 0.0001) hist4++;
            else if (d < 0.001) hist3++;
            else if (d < 0.01) hist2++;
            else if (d < 0.1) hist1++;
            else if (d < 1.) hist0++;
            else histN++;
        }
    }
    else if (_membuf64)
    {
        const size_t len = _membuf64->length();
        for (size_t i = 0; i < len; i++)
        {
            const double elem1 = (*_membuf64)[i];
            const double elem2 = _hostarray64[i];
            const double d = fabs(elem1 - elem2);
                 if (d < 0.0000001) hist7++;
            else if (d < 0.000001) hist6++;
            else if (d < 0.00001) hist5++;
            else if (d < 0.0001) hist4++;
            else if (d < 0.001) hist3++;
            else if (d < 0.01) hist2++;
            else if (d < 0.1) hist1++;
            else if (d < 1.) hist0++;
            else histN++;
        }
    }
    else if (_imgbuf32)
    {
        const size_t len = _imgbuf32->width() * _imgbuf32->height();
        for (size_t i = 0; i < len; i++)
        {
            const double elem1 = (*_imgbuf32)[i];
            const double elem2 = _hostarray32[i];
            const double d = fabs(elem1 - elem2);
                 if (d < 0.0000001) hist7++;
            else if (d < 0.000001) hist6++;
            else if (d < 0.00001) hist5++;
            else if (d < 0.0001) hist4++;
            else if (d < 0.001) hist3++;
            else if (d < 0.01) hist2++;
            else if (d < 0.1) hist1++;
            else if (d < 1.) hist0++;
            else histN++;
        }
    }
    else if (_imgbuf64)
    {
        const size_t len = _imgbuf64->width() * _imgbuf64->height();
        for (size_t i = 0; i < len; i++)
        {
            const double elem1 = (*_imgbuf64)[i];
            const double elem2 = _hostarray64[i];
            const double d = fabs(elem1 - elem2);
                 if (d < 0.0000001) hist7++;
            else if (d < 0.000001) hist6++;
            else if (d < 0.00001) hist5++;
            else if (d < 0.0001) hist4++;
            else if (d < 0.001) hist3++;
            else if (d < 0.01) hist2++;
            else if (d < 0.1) hist1++;
            else if (d < 1.) hist0++;
            else histN++;
        }
    }

    map< int, size_t > errorHist;
    errorHist[1] = histN;
    errorHist[0] = hist0;
    errorHist[-1] = hist1;
    errorHist[-2] = hist2;
    errorHist[-3] = hist3;
    errorHist[-4] = hist4;
    errorHist[-5] = hist5;
    errorHist[-6] = hist6;
    errorHist[-7] = hist7;

    return errorHist;
}

void ArrayBuf::fillValue(const float val32)
{
    if (_membuf32)
    {
        const size_t len = _membuf32->length();
        fillconst<float>(static_cast<float*>(*_membuf32), val32, len);
        if (_hostarray32)
            memcpy(_hostarray32,
                   static_cast<const float*>(*_membuf32),
                   len * sizeof(float));
    }
    else if (_imgbuf32)
    {
        const size_t len = _imgbuf32->width() * _imgbuf32->height();
        fillconst<float>(static_cast<float*>(*_imgbuf32), val32, len);
        if (_hostarray32)
            memcpy(_hostarray32,
                   static_cast<const float*>(*_imgbuf32),
                   len * sizeof(float));
    }
}

void ArrayBuf::fillValue(const double val64)
{
    if (_membuf64)
    {
        const size_t len = _membuf64->length();
        fillconst<double>(static_cast<double*>(*_membuf64), val64, len);
        if (_hostarray64)
            memcpy(_hostarray64,
                   static_cast<const double*>(*_membuf64),
                   len * sizeof(double));
    }
    else if (_imgbuf64)
    {
        const size_t len = _imgbuf64->width() * _imgbuf64->height();
        fillconst<double>(static_cast<double*>(*_imgbuf64), val64, len);
        if (_hostarray64)
            memcpy(_hostarray64,
                   static_cast<const double*>(*_imgbuf64),
                   len * sizeof(double));
    }
}

void ArrayBuf::fillZeros(void)
{
    if (_membuf32)
    {
        const size_t len = _membuf32->length();
        fillconst<float>(static_cast<float*>(*_membuf32), 0, len);
        if (_hostarray32)
            memcpy(_hostarray32,
                   static_cast<const float*>(*_membuf32),
                   len * sizeof(float));
    }
    else if (_membuf64)
    {
        const size_t len = _membuf64->length();
        fillconst<double>(static_cast<double*>(*_membuf64), 0, len);
        if (_hostarray64)
            memcpy(_hostarray64,
                   static_cast<const double*>(*_membuf64),
                   len * sizeof(double));
    }
    else if (_imgbuf32)
    {
        const size_t len = _imgbuf32->width() * _imgbuf32->height();
        fillconst<float>(static_cast<float*>(*_imgbuf32), 0, len);
        if (_hostarray32)
            memcpy(_hostarray32,
                   static_cast<const float*>(*_imgbuf32),
                   len * sizeof(float));
    }
    else if (_imgbuf64)
    {
        const size_t len = _imgbuf64->width() * _imgbuf64->height();
        fillconst<double>(static_cast<double*>(*_imgbuf64), 0, len);
        if (_hostarray64)
            memcpy(_hostarray64,
                   static_cast<const double*>(*_imgbuf64),
                   len * sizeof(double));
    }
}

void ArrayBuf::fillOnes(void)
{
    if (_membuf32)
    {
        const size_t len = _membuf32->length();
        fillconst<float>(static_cast<float*>(*_membuf32), 1, len);
        if (_hostarray32)
            memcpy(_hostarray32,
                   static_cast<const float*>(*_membuf32),
                   len * sizeof(float));
    }
    else if (_membuf64)
    {
        const size_t len = _membuf64->length();
        fillconst<double>(static_cast<double*>(*_membuf64), 1, len);
        if (_hostarray64)
            memcpy(_hostarray64,
                   static_cast<const double*>(*_membuf64),
                   len * sizeof(double));
    }
    else if (_imgbuf32)
    {
        const size_t len = _imgbuf32->width() * _imgbuf32->height();
        fillconst<float>(static_cast<float*>(*_imgbuf32), 1, len);
        if (_hostarray32)
            memcpy(_hostarray32,
                   static_cast<const float*>(*_imgbuf32),
                   len * sizeof(float));
    }
    else if (_imgbuf64)
    {
        const size_t len = _imgbuf64->width() * _imgbuf64->height();
        fillconst<double>(static_cast<double*>(*_imgbuf64), 1, len);
        if (_hostarray64)
            memcpy(_hostarray64,
                   static_cast<const double*>(*_imgbuf64),
                   len * sizeof(double));
    }
}

void ArrayBuf::fillRandom(void)
{
    if (_membuf32)
    {
        const size_t len = _membuf32->length();
        fillrand<float>(static_cast<float*>(*_membuf32), len);
        if (_hostarray32)
            memcpy(_hostarray32,
                   static_cast<const float*>(*_membuf32),
                   len * sizeof(float));
    }
    else if (_membuf64)
    {
        const size_t len = _membuf64->length();
        fillrand<double>(static_cast<double*>(*_membuf64), len);
        if (_hostarray64)
            memcpy(_hostarray64,
                   static_cast<const double*>(*_membuf64),
                   len * sizeof(double));
    }
    else if (_imgbuf32)
    {
        const size_t len = _imgbuf32->width() * _imgbuf32->height();
        fillrand<float>(static_cast<float*>(*_imgbuf32), len);
        if (_hostarray32)
            memcpy(_hostarray32,
                   static_cast<const float*>(*_imgbuf32),
                   len * sizeof(float));
    }
    else if (_imgbuf64)
    {
        const size_t len = _imgbuf64->width() * _imgbuf64->height();
        fillrand<double>(static_cast<double*>(*_imgbuf64), len);
        if (_hostarray64)
            memcpy(_hostarray64,
                   static_cast<const double*>(*_imgbuf64),
                   len * sizeof(double));
    }
}

bool ArrayBuf::flushToDevice(void)
{
    if (_membuf32)
        _cdev << *_membuf32 << FLUSH;

    else if (_membuf64)
        _cdev << *_membuf64 << FLUSH;

    else if (_imgbuf32)
        _cdev << *_imgbuf32 << FLUSH;

    else if (_imgbuf64)
        _cdev << *_imgbuf64 << FLUSH;

    return _cdev.statusOp();
}

bool ArrayBuf::flushFromDevice(void)
{
    if (_membuf32)
        _cdev >> *_membuf32 >> FLUSH;

    else if (_membuf64)
        _cdev >> *_membuf64 >> FLUSH;

    else if (_imgbuf32)
        _cdev >> *_imgbuf32 >> FLUSH;

    else if (_imgbuf64)
        _cdev >> *_imgbuf64 >> FLUSH;

    return _cdev.statusOp();
}

bool ArrayBuf::argumentToKernel(OCLkernel& ckernel)
{
    if (_membuf32)
    {
        ckernel << *_membuf32;
    }
    else if (_membuf64)
    {
        ckernel << *_membuf64;
    }
    else if (_imgbuf32)
    {
        ckernel << *_imgbuf32;
    }
    else if (_imgbuf64)
    {
        ckernel << *_imgbuf64;
    }

    return ckernel.statusOp();
}

}; // namespace chai_internal
