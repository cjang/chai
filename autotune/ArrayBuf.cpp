// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <math.h>
#include <string.h>

#include "ArrayBuf.hpp"
#include "ArrayBufUtil.hpp"
#include "PrecType.hpp"

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
        case (PrecType::UInt32) :
            switch (vectorLength)
            {
                case (0) :

                    _imgbufUInt32 = new OCLimgbuf<uint32_t>(
                                            cdev,
                                            width,
                                            packing * height,
                                            imgbufMode(mode),
                                            pinned);

                    if (_imgbufUInt32 && _imgbufUInt32->isOk())
                    {
                        _devbuf = IBU;
                    }
                    else
                    {
                        delete _imgbufUInt32;
                        _imgbufUInt32 = NULL;
                    }

                    if (hostarray)
                        _hostarrayUInt32 = new uint32_t[packing *
                                                        width *
                                                        height];

                    break;

                case (1) :
                case (2) :
                case (4) :

                    _membufUInt32 = new OCLmembuf<uint32_t>(
                                            cdev,
                                            packing * width * height,
                                            membufMode(mode),
                                            pinned,
                                            vectorLength);

                    if (_membufUInt32 && _membufUInt32->isOk())
                    {
                        _devbuf = MBU;
                    }
                    else
                    {
                        delete _membufUInt32;
                        _membufUInt32 = NULL;
                    }

                    if (hostarray)
                        _hostarrayUInt32 = new uint32_t[packing *
                                                        width *
                                                        height];

                    break;
            }
            break;

        case (PrecType::Int32) :
            switch (vectorLength)
            {
                case (0) :

                    _imgbufInt32 = new OCLimgbuf<int32_t>(
                                           cdev,
                                           width,
                                           packing * height,
                                           imgbufMode(mode),
                                           pinned);

                    if (_imgbufInt32 && _imgbufInt32->isOk())
                    {
                        _devbuf = IBI;
                    }
                    else
                    {
                        delete _imgbufInt32;
                        _imgbufInt32 = NULL;
                    }

                    if (hostarray)
                        _hostarrayInt32 = new int32_t[packing *
                                                      width *
                                                      height];

                    break;

                case (1) :
                case (2) :
                case (4) :

                    _membufInt32 = new OCLmembuf<int32_t>(
                                           cdev,
                                           packing * width * height,
                                           membufMode(mode),
                                           pinned,
                                           vectorLength);

                    if (_membufInt32 && _membufInt32->isOk())
                    {
                        _devbuf = MBI;
                    }
                    else
                    {
                        delete _membufInt32;
                        _membufInt32 = NULL;
                    }

                    if (hostarray)
                        _hostarrayInt32 = new int32_t[packing *
                                                      width *
                                                      height];

                    break;
            }
            break;

        case (PrecType::Float) :
            switch (vectorLength)
            {
                case (0) :

                    _imgbufFloat = new OCLimgbuf<float>(
                                           cdev,
                                           width,
                                           packing * height,
                                           imgbufMode(mode),
                                           pinned);

                    if (_imgbufFloat && _imgbufFloat->isOk())
                    {
                        _devbuf = IBF;
                    }
                    else
                    {
                        delete _imgbufFloat;
                        _imgbufFloat = NULL;
                    }

                    if (hostarray)
                        _hostarrayFloat = new float[packing *
                                                    width *
                                                    height];

                    break;

                case (1) :
                case (2) :
                case (4) :

                    _membufFloat = new OCLmembuf<float>(
                                           cdev,
                                           packing * width * height,
                                           membufMode(mode),
                                           pinned,
                                           vectorLength);

                    if (_membufFloat && _membufFloat->isOk())
                    {
                        _devbuf = MBF;
                    }
                    else
                    {
                        delete _membufFloat;
                        _membufFloat = NULL;
                    }

                    if (hostarray)
                        _hostarrayFloat = new float[packing *
                                                    width *
                                                    height];

                    break;
            }
            break;

        case (PrecType::Double) :
            switch (vectorLength)
            {
                case (0) :

                    _imgbufDouble = new OCLimgbuf<double>(
                                            cdev,
                                            width,
                                            packing * height,
                                            imgbufMode(mode),
                                            pinned);

                    if (_imgbufDouble && _imgbufDouble->isOk())
                    {
                        _devbuf = IBD;
                    }
                    else
                    {
                        delete _imgbufDouble;
                        _imgbufDouble = NULL;
                    }

                    if (hostarray)
                        _hostarrayDouble = new double[packing *
                                                      width *
                                                      height];

                    break;

                case (1) :
                case (2) :
                case (4) :

                    _membufDouble = new OCLmembuf<double>(
                                            cdev,
                                            packing * width * height,
                                            membufMode(mode),
                                            pinned,
                                            vectorLength);

                    if (_membufDouble && _membufDouble->isOk())
                    {
                        _devbuf = MBD;
                    }
                    else
                    {
                        delete _membufDouble;
                        _membufDouble = NULL;
                    }

                    if (hostarray)
                        _hostarrayDouble = new double[packing *
                                                      width *
                                                      height];

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
                _imgbufFloat = new OCLimgbuf<float>(
                                       cdev,
                                       width,
                                       packing * height,
                                       static_cast<const float*>(ptr) );
            }
            else if (WRITE == mode)
            {
                _imgbufFloat = new OCLimgbuf<float>(
                                       cdev,
                                       width,
                                       packing * height,
                                       ptr );
            }

            if (_imgbufFloat && _imgbufFloat->isOk())
            {
                _devbuf = IBF;
            }
            else
            {
                delete _imgbufFloat;
                _imgbufFloat = NULL;
            }

            if (hostarray)
                _hostarrayFloat = new float[packing * width * height];

            break;

        case (1) :
        case (2) :
        case (4) :

            if (READ == mode)
            {
                _membufFloat = new OCLmembuf<float>(
                                       cdev,
                                       packing * width * height,
                                       static_cast<const float*>(ptr) );
            }
            else
            {
                _membufFloat = new OCLmembuf<float>(
                                       cdev,
                                       packing * width * height,
                                       ptr );
            }

            if (_membufFloat && _membufFloat->isOk())
            {
                _devbuf = MBF;
            }
            else
            {
                delete _membufFloat;
                _membufFloat = NULL;
            }

            if (hostarray)
                _hostarrayFloat = new float[packing * width * height];

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
                _imgbufDouble = new OCLimgbuf<double>(
                                        cdev,
                                        width,
                                        packing * height,
                                        static_cast<const double*>(ptr) );
            }
            else if (WRITE == mode)
            {
                _imgbufDouble = new OCLimgbuf<double>(
                                        cdev,
                                        width,
                                        packing * height,
                                        ptr );
            }

            if (_imgbufDouble && _imgbufDouble->isOk())
            {
                _devbuf = IBD;
            }
            else
            {
                delete _imgbufDouble;
                _imgbufDouble = NULL;
            }

            if (hostarray)
                _hostarrayDouble = new double[packing * width * height];

            break;

        case (1) :
        case (2) :
        case (4) :

            if (READ == mode)
            {
                _membufDouble = new OCLmembuf<double>(
                                        cdev,
                                        packing * width * height,
                                        static_cast<const double*>(ptr) );
            }
            else
            {
                _membufDouble = new OCLmembuf<double>(
                                        cdev,
                                        packing * width * height,
                                        ptr );
            }

            if (_membufDouble && _membufDouble->isOk())
            {
                _devbuf = MBD;
            }
            else
            {
                delete _membufDouble;
                _membufDouble = NULL;
            }

            if (hostarray)
                _hostarrayDouble = new double[packing * width * height];

            break;
    }
}

void ArrayBuf::initBuf(OCLdevice& cdev,
                       const int mode,
                       const size_t packing,
                       const size_t width,
                       const size_t height,
                       int32_t* ptr,
                       const size_t vectorLength,
                       const bool hostarray)
{
    switch (vectorLength)
    {
        case (0) :

            if (READ == mode)
            {
                _imgbufInt32 = new OCLimgbuf<int32_t>(
                                       cdev,
                                       width,
                                       packing * height,
                                       static_cast<const int32_t*>(ptr) );
            }
            else if (WRITE == mode)
            {
                _imgbufInt32 = new OCLimgbuf<int32_t>(
                                       cdev,
                                       width,
                                       packing * height,
                                       ptr );
            }

            if (_imgbufInt32 && _imgbufInt32->isOk())
            {
                _devbuf = IBI;
            }
            else
            {
                delete _imgbufInt32;
                _imgbufInt32 = NULL;
            }

            if (hostarray)
                _hostarrayInt32 = new int32_t[packing * width * height];

            break;

        case (1) :
        case (2) :
        case (4) :

            if (READ == mode)
            {
                _membufInt32 = new OCLmembuf<int32_t>(
                                       cdev,
                                       packing * width * height,
                                       static_cast<const int32_t*>(ptr) );
            }
            else
            {
                _membufInt32 = new OCLmembuf<int32_t>(
                                       cdev,
                                       packing * width * height,
                                       ptr );
            }

            if (_membufInt32 && _membufInt32->isOk())
            {
                _devbuf = MBI;
            }
            else
            {
                delete _membufInt32;
                _membufInt32 = NULL;
            }

            if (hostarray)
                _hostarrayInt32 = new int32_t[packing * width * height];

            break;
    }
}

void ArrayBuf::initBuf(OCLdevice& cdev,
                       const int mode,
                       const size_t packing,
                       const size_t width,
                       const size_t height,
                       uint32_t* ptr,
                       const size_t vectorLength,
                       const bool hostarray)
{
    switch (vectorLength)
    {
        case (0) :

            if (READ == mode)
            {
                _imgbufUInt32 = new OCLimgbuf<uint32_t>(
                                        cdev,
                                        width,
                                        packing * height,
                                        static_cast<const uint32_t*>(ptr) );
            }
            else if (WRITE == mode)
            {
                _imgbufUInt32 = new OCLimgbuf<uint32_t>(
                                        cdev,
                                        width,
                                        packing * height,
                                        ptr );
            }

            if (_imgbufUInt32 && _imgbufUInt32->isOk())
            {
                _devbuf = IBU;
            }
            else
            {
                delete _imgbufUInt32;
                _imgbufUInt32 = NULL;
            }

            if (hostarray)
                _hostarrayUInt32 = new uint32_t[packing * width * height];

            break;

        case (1) :
        case (2) :
        case (4) :

            if (READ == mode)
            {
                _membufUInt32 = new OCLmembuf<uint32_t>(
                                        cdev,
                                        packing * width * height,
                                        static_cast<const uint32_t*>(ptr) );
            }
            else
            {
                _membufUInt32 = new OCLmembuf<uint32_t>(
                                        cdev,
                                        packing * width * height,
                                        ptr );
            }

            if (_membufUInt32 && _membufUInt32->isOk())
            {
                _devbuf = MBU;
            }
            else
            {
                delete _membufUInt32;
                _membufUInt32 = NULL;
            }

            if (hostarray)
                _hostarrayUInt32 = new uint32_t[packing * width * height];

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
      _devbuf(UNSET),
      _membufFloat(NULL),
      _membufDouble(NULL),
      _membufInt32(NULL),
      _membufUInt32(NULL),
      _imgbufFloat(NULL),
      _imgbufDouble(NULL),
      _imgbufInt32(NULL),
      _imgbufUInt32(NULL),
      _hostarrayFloat(NULL),
      _hostarrayDouble(NULL),
      _hostarrayInt32(NULL),
      _hostarrayUInt32(NULL)
{
    initBuf(cdev,
            mode,
            packing,
            width,
            height,
            _precision,
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
      _precision(PrecType::Float),
      _vectorLength(vectorLength),
      _devbuf(UNSET),
      _membufFloat(NULL),
      _membufDouble(NULL),
      _membufInt32(NULL),
      _membufUInt32(NULL),
      _imgbufFloat(NULL),
      _imgbufDouble(NULL),
      _imgbufInt32(NULL),
      _imgbufUInt32(NULL),
      _hostarrayFloat(NULL),
      _hostarrayDouble(NULL),
      _hostarrayInt32(NULL),
      _hostarrayUInt32(NULL)
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
      _precision(PrecType::Double),
      _vectorLength(vectorLength),
      _devbuf(UNSET),
      _membufFloat(NULL),
      _membufDouble(NULL),
      _membufInt32(NULL),
      _membufUInt32(NULL),
      _imgbufFloat(NULL),
      _imgbufDouble(NULL),
      _imgbufInt32(NULL),
      _imgbufUInt32(NULL),
      _hostarrayFloat(NULL),
      _hostarrayDouble(NULL),
      _hostarrayInt32(NULL),
      _hostarrayUInt32(NULL)
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
                   int32_t* ptr,
                   const size_t vectorLength,
                   const bool hostarray)
    : _cdev(cdev),
      _packing(packing),
      _width(width),
      _height(height),
      _precision(PrecType::Int32),
      _vectorLength(vectorLength),
      _devbuf(UNSET),
      _membufFloat(NULL),
      _membufDouble(NULL),
      _membufInt32(NULL),
      _membufUInt32(NULL),
      _imgbufFloat(NULL),
      _imgbufDouble(NULL),
      _imgbufInt32(NULL),
      _imgbufUInt32(NULL),
      _hostarrayFloat(NULL),
      _hostarrayDouble(NULL),
      _hostarrayInt32(NULL),
      _hostarrayUInt32(NULL)
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
                   uint32_t* ptr,
                   const size_t vectorLength,
                   const bool hostarray)
    : _cdev(cdev),
      _packing(packing),
      _width(width),
      _height(height),
      _precision(PrecType::UInt32),
      _vectorLength(vectorLength),
      _devbuf(UNSET),
      _membufFloat(NULL),
      _membufDouble(NULL),
      _membufInt32(NULL),
      _membufUInt32(NULL),
      _imgbufFloat(NULL),
      _imgbufDouble(NULL),
      _imgbufInt32(NULL),
      _imgbufUInt32(NULL),
      _hostarrayFloat(NULL),
      _hostarrayDouble(NULL),
      _hostarrayInt32(NULL),
      _hostarrayUInt32(NULL)
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
    delete _membufFloat;
    delete _membufDouble;
    delete _membufInt32;
    delete _membufUInt32;
    delete _imgbufFloat;
    delete _imgbufDouble;
    delete _imgbufInt32;
    delete _imgbufUInt32;
    delete[] _hostarrayFloat;
    delete[] _hostarrayDouble;
    delete[] _hostarrayInt32;
    delete[] _hostarrayUInt32;
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

const size_t ArrayBuf::precision(void) const
{
    return _precision;
}

size_t ArrayBuf::vectorLength(void) const
{
    return _vectorLength;
}

bool ArrayBuf::bufOk(void) const
{
    return UNSET != _devbuf;
}

bool ArrayBuf::isImgbuf(void) const
{
    switch (_devbuf)
    {
        case (IBF) :
        case (IBD) :
        case (IBI) :
        case (IBU) :
            return true;

        default :
            return false;
    }
}

bool ArrayBuf::isMembuf(void) const
{
    switch (_devbuf)
    {
        case (MBF) :
        case (MBD) :
        case (MBI) :
        case (MBU) :
            return true;

        default :
            return false;
    }
}

double ArrayBuf::getArrayElem(const size_t absoluteRow,
                              const size_t col) const
{
    const size_t idx = absoluteRow * _width + col;

    switch (_devbuf)
    {
        case (MBF) : return (*_membufFloat)[idx];
        case (MBD) : return (*_membufDouble)[idx];
        case (MBI) : return (*_membufInt32)[idx];
        case (MBU) : return (*_membufUInt32)[idx];
        case (IBF) : return (*_imgbufFloat)[idx];
        case (IBD) : return (*_imgbufDouble)[idx];
        case (IBI) : return (*_imgbufInt32)[idx];
        case (IBU) : return (*_imgbufUInt32)[idx];
        default : return 0;
    }
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

    switch (_devbuf)
    {
        case (MBF) :
        case (IBF) :
            return _hostarrayFloat[idx];

        case (MBD) :
        case (IBD) :
            return _hostarrayDouble[idx];

        case (MBI) :
        case (IBI) :
            return _hostarrayInt32[idx];

        case (MBU) :
        case (IBU) :
            return _hostarrayUInt32[idx];
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

    switch (_devbuf)
    {
        case (MBF) : (*_membufFloat)[idx] = value; break;
        case (MBD) : (*_membufDouble)[idx] = value; break;
        case (MBI) : (*_membufInt32)[idx] = value; break;
        case (MBU) : (*_membufUInt32)[idx] = value; break;
        case (IBF) : (*_imgbufFloat)[idx] = value; break;
        case (IBD) : (*_imgbufDouble)[idx] = value; break;
        case (IBI) : (*_imgbufInt32)[idx] = value; break;
        case (IBU) : (*_imgbufUInt32)[idx] = value; break;
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

    switch (_devbuf)
    {
        case (MBF) :
        case (IBF) :
            _hostarrayFloat[idx] = value;
            break;

        case (MBD) :
        case (IBD) :
            _hostarrayDouble[idx] = value;
            break;

        case (MBI) :
        case (IBI) :
            _hostarrayInt32[idx] = value;
            break;

        case (MBU) :
        case (IBU) :
            _hostarrayUInt32[idx] = value;
            break;
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

    switch (_devbuf)
    {
        case (MBF) :
            {
            const size_t len = _membufFloat->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem = (*_membufFloat)[i];
                if (! histMap.count(elem))
                    histMap[elem] = 1;
                else
                    histMap[elem]++;
            }
            }
            break;

        case (MBD) :
            {
            const size_t len = _membufDouble->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem = (*_membufDouble)[i];
                if (! histMap.count(elem))
                    histMap[elem] = 1;
                else
                    histMap[elem]++;
            }
            }
            break;

        case (MBI) :
            {
            const size_t len = _membufInt32->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem = (*_membufInt32)[i];
                if (! histMap.count(elem))
                    histMap[elem] = 1;
                else
                    histMap[elem]++;
            }
            }
            break;

        case (MBU) :
            {
            const size_t len = _membufUInt32->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem = (*_membufUInt32)[i];
                if (! histMap.count(elem))
                    histMap[elem] = 1;
                else
                    histMap[elem]++;
            }
            }
            break;

        case (IBF) :
            {
            const size_t len = _imgbufFloat->width() * _imgbufFloat->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem = (*_imgbufFloat)[i];
                if (! histMap.count(elem))
                    histMap[elem] = 1;
                else
                    histMap[elem]++;
            }
            }
            break;

        case (IBD) :
            {
            const size_t len = _imgbufDouble->width() * _imgbufDouble->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem = (*_imgbufDouble)[i];
                if (! histMap.count(elem))
                    histMap[elem] = 1;
                else
                    histMap[elem]++;
            }
            }
            break;

        case (IBI) :
            {
            const size_t len = _imgbufInt32->width() * _imgbufInt32->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem = (*_imgbufInt32)[i];
                if (! histMap.count(elem))
                    histMap[elem] = 1;
                else
                    histMap[elem]++;
            }
            }
            break;

        case (IBU) :
            {
            const size_t len = _imgbufUInt32->width() * _imgbufUInt32->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem = (*_imgbufUInt32)[i];
                if (! histMap.count(elem))
                    histMap[elem] = 1;
                else
                    histMap[elem]++;
            }
            }
            break;
    }

    return histMap;
}

double ArrayBuf::absdiffHostArray(void) const
{
    double accumDiff = 0;

    switch (_devbuf)
    {
        case (MBF) :
            {
            const size_t len = _membufFloat->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_membufFloat)[i];
                const double elem2 = _hostarrayFloat[i];
                accumDiff += fabs(elem1 - elem2);
            }
            }
            break;

        case (MBD) :
            {
            const size_t len = _membufDouble->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_membufDouble)[i];
                const double elem2 = _hostarrayDouble[i];
                accumDiff += fabs(elem1 - elem2);
            }
            }
            break;

        case (MBI) :
            {
            const size_t len = _membufInt32->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_membufInt32)[i];
                const double elem2 = _hostarrayInt32[i];
                accumDiff += fabs(elem1 - elem2);
            }
            }
            break;

        case (MBU) :
            {
            const size_t len = _membufUInt32->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_membufUInt32)[i];
                const double elem2 = _hostarrayUInt32[i];
                accumDiff += fabs(elem1 - elem2);
            }
            }
            break;

        case (IBF) :
            {
            const size_t len = _imgbufFloat->width() * _imgbufFloat->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_imgbufFloat)[i];
                const double elem2 = _hostarrayFloat[i];
                accumDiff += fabs(elem1 - elem2);
            }
            }
            break;

        case (IBD) :
            {
            const size_t len = _imgbufDouble->width() * _imgbufDouble->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_imgbufDouble)[i];
                const double elem2 = _hostarrayDouble[i];
                accumDiff += fabs(elem1 - elem2);
            }
            }
            break;

        case (IBI) :
            {
            const size_t len = _imgbufInt32->width() * _imgbufInt32->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_imgbufInt32)[i];
                const double elem2 = _hostarrayInt32[i];
                accumDiff += fabs(elem1 - elem2);
            }
            }
            break;

        case (IBU) :
            {
            const size_t len = _imgbufUInt32->width() * _imgbufUInt32->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_imgbufUInt32)[i];
                const double elem2 = _hostarrayUInt32[i];
                accumDiff += fabs(elem1 - elem2);
            }
            }
            break;
    }

    return accumDiff;
}

map< int, size_t > ArrayBuf::absdiffHistogram(void) const
{
    size_t histN = 0, hist0 = 0, hist1 = 0, hist2 = 0,
           hist3 = 0, hist4 = 0, hist5 = 0, hist6 = 0,
           hist7 = 0;

    switch (_devbuf)
    {
        case (MBF) :
            {
            const size_t len = _membufFloat->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_membufFloat)[i];
                const double elem2 = _hostarrayFloat[i];
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
            break;

        case (MBD) :
            {
            const size_t len = _membufDouble->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_membufDouble)[i];
                const double elem2 = _hostarrayDouble[i];
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
            break;

        case (MBI) :
            {
            const size_t len = _membufInt32->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_membufInt32)[i];
                const double elem2 = _hostarrayInt32[i];
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
            break;

        case (MBU) :
            {
            const size_t len = _membufUInt32->length();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_membufUInt32)[i];
                const double elem2 = _hostarrayUInt32[i];
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
            break;

        case (IBF) :
            {
            const size_t len = _imgbufFloat->width() * _imgbufFloat->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_imgbufFloat)[i];
                const double elem2 = _hostarrayFloat[i];
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
            break;

        case (IBD) :
            {
            const size_t len = _imgbufDouble->width() * _imgbufDouble->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_imgbufDouble)[i];
                const double elem2 = _hostarrayDouble[i];
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
            break;

        case (IBI) :
            {
            const size_t len = _imgbufInt32->width() * _imgbufInt32->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_imgbufInt32)[i];
                const double elem2 = _hostarrayInt32[i];
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
            break;

        case (IBU) :
            {
            const size_t len = _imgbufUInt32->width() * _imgbufUInt32->height();
            for (size_t i = 0; i < len; i++)
            {
                const double elem1 = (*_imgbufUInt32)[i];
                const double elem2 = _hostarrayUInt32[i];
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
            break;
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

void ArrayBuf::fillValue(const float val)
{
    switch (_devbuf)
    {
        case (MBF) :
            {
            const size_t len = _membufFloat->length();
            fillconst<float>(static_cast<float*>(*_membufFloat), val, len);
            if (_hostarrayFloat)
                memcpy(_hostarrayFloat,
                       static_cast<const float*>(*_membufFloat),
                       len * sizeof(float));
            }
            break;

        case (IBF) :
            {
            const size_t len = _imgbufFloat->width() * _imgbufFloat->height();
            fillconst<float>(static_cast<float*>(*_imgbufFloat), val, len);
            if (_hostarrayFloat)
                memcpy(_hostarrayFloat,
                       static_cast<const float*>(*_imgbufFloat),
                       len * sizeof(float));
            }
            break;
    }
}

void ArrayBuf::fillValue(const double val)
{
    switch (_devbuf)
    {
        case (MBD) :
            {
            const size_t len = _membufDouble->length();
            fillconst<double>(static_cast<double*>(*_membufDouble), val, len);
            if (_hostarrayDouble)
                memcpy(_hostarrayDouble,
                       static_cast<const double*>(*_membufDouble),
                       len * sizeof(double));
            }
            break;

        case (IBD) :
            {
            const size_t len = _imgbufDouble->width() * _imgbufDouble->height();
            fillconst<double>(static_cast<double*>(*_imgbufDouble), val, len);
            if (_hostarrayDouble)
                memcpy(_hostarrayDouble,
                       static_cast<const double*>(*_imgbufDouble),
                       len * sizeof(double));
            }
            break;
    }
}

void ArrayBuf::fillValue(const int32_t val)
{
    switch (_devbuf)
    {
        case (MBI) :
            {
            const size_t len = _membufInt32->length();
            fillconst<int32_t>(static_cast<int32_t*>(*_membufInt32), val, len);
            if (_hostarrayInt32)
                memcpy(_hostarrayInt32,
                       static_cast<const int32_t*>(*_membufInt32),
                       len * sizeof(int32_t));
            }
            break;

        case (IBI) :
            {
            const size_t len = _imgbufInt32->width() * _imgbufInt32->height();
            fillconst<int32_t>(static_cast<int32_t*>(*_imgbufInt32), val, len);
            if (_hostarrayInt32)
                memcpy(_hostarrayInt32,
                       static_cast<const int32_t*>(*_imgbufInt32),
                       len * sizeof(int32_t));
            }
            break;
    }
}

void ArrayBuf::fillValue(const uint32_t val)
{
    switch (_devbuf)
    {
        case (MBU) :
            {
            const size_t len = _membufUInt32->length();
            fillconst<uint32_t>( static_cast<uint32_t*>(*_membufUInt32),
                                 val,
                                 len );
            if (_hostarrayUInt32)
                memcpy(_hostarrayUInt32,
                       static_cast<const uint32_t*>(*_membufUInt32),
                       len * sizeof(uint32_t));
            }
            break;

        case (IBU) :
            {
            const size_t len = _imgbufUInt32->width() * _imgbufUInt32->height();
            fillconst<uint32_t>( static_cast<uint32_t*>(*_imgbufUInt32),
                                 val,
                                 len );
            if (_hostarrayUInt32)
                memcpy(_hostarrayUInt32,
                       static_cast<const uint32_t*>(*_imgbufUInt32),
                       len * sizeof(uint32_t));
            }
            break;
    }
}

void ArrayBuf::fillZeros(void)
{
    switch (_devbuf)
    {
        case (MBF) :
            {
            const size_t len = _membufFloat->length();
            fillconst<float>(static_cast<float*>(*_membufFloat), 0, len);
            if (_hostarrayFloat)
                memcpy(_hostarrayFloat,
                       static_cast<const float*>(*_membufFloat),
                       len * sizeof(float));
            }
            break;

        case (MBD) :
            {
            const size_t len = _membufDouble->length();
            fillconst<double>(static_cast<double*>(*_membufDouble), 0, len);
            if (_hostarrayDouble)
                memcpy(_hostarrayDouble,
                       static_cast<const double*>(*_membufDouble),
                       len * sizeof(double));
            }
            break;

        case (MBI) :
            {
            const size_t len = _membufInt32->length();
            fillconst<int32_t>(static_cast<int32_t*>(*_membufInt32), 0, len);
            if (_hostarrayInt32)
                memcpy(_hostarrayInt32,
                       static_cast<const int32_t*>(*_membufInt32),
                       len * sizeof(int32_t));
            }
            break;

        case (MBU) :
            {
            const size_t len = _membufUInt32->length();
            fillconst<uint32_t>(static_cast<uint32_t*>(*_membufUInt32), 0, len);
            if (_hostarrayUInt32)
                memcpy(_hostarrayUInt32,
                       static_cast<const uint32_t*>(*_membufUInt32),
                       len * sizeof(uint32_t));
            }
            break;

        case (IBF) :
            {
            const size_t len = _imgbufFloat->width() * _imgbufFloat->height();
            fillconst<float>(static_cast<float*>(*_imgbufFloat), 0, len);
            if (_hostarrayFloat)
                memcpy(_hostarrayFloat,
                       static_cast<const float*>(*_imgbufFloat),
                       len * sizeof(float));
            }
            break;

        case (IBD) :
            {
            const size_t len = _imgbufDouble->width() * _imgbufDouble->height();
            fillconst<double>(static_cast<double*>(*_imgbufDouble), 0, len);
            if (_hostarrayDouble)
                memcpy(_hostarrayDouble,
                       static_cast<const double*>(*_imgbufDouble),
                       len * sizeof(double));
            }
            break;

        case (IBI) :
            {
            const size_t len = _imgbufInt32->width() * _imgbufInt32->height();
            fillconst<int32_t>(static_cast<int32_t*>(*_imgbufInt32), 0, len);
            if (_hostarrayInt32)
                memcpy(_hostarrayInt32,
                       static_cast<const int32_t*>(*_imgbufInt32),
                       len * sizeof(int32_t));
            }
            break;

        case (IBU) :
            {
            const size_t len = _imgbufUInt32->width() * _imgbufUInt32->height();
            fillconst<uint32_t>(static_cast<uint32_t*>(*_imgbufUInt32), 0, len);
            if (_hostarrayUInt32)
                memcpy(_hostarrayUInt32,
                       static_cast<const uint32_t*>(*_imgbufUInt32),
                       len * sizeof(uint32_t));
            }
            break;
    }
}

void ArrayBuf::fillOnes(void)
{
    switch (_devbuf)
    {
        case (MBF) :
            {
            const size_t len = _membufFloat->length();
            fillconst<float>(static_cast<float*>(*_membufFloat), 1, len);
            if (_hostarrayFloat)
                memcpy(_hostarrayFloat,
                       static_cast<const float*>(*_membufFloat),
                       len * sizeof(float));
            }
            break;

        case (MBD) :
            {
            const size_t len = _membufDouble->length();
            fillconst<double>(static_cast<double*>(*_membufDouble), 1, len);
            if (_hostarrayDouble)
                memcpy(_hostarrayDouble,
                       static_cast<const double*>(*_membufDouble),
                       len * sizeof(double));
            }
            break;

        case (MBI) :
            {
            const size_t len = _membufInt32->length();
            fillconst<int32_t>(static_cast<int32_t*>(*_membufInt32), 1, len);
            if (_hostarrayInt32)
                memcpy(_hostarrayInt32,
                       static_cast<const int32_t*>(*_membufInt32),
                       len * sizeof(int32_t));
            }
            break;

        case (MBU) :
            {
            const size_t len = _membufUInt32->length();
            fillconst<uint32_t>(static_cast<uint32_t*>(*_membufUInt32), 1, len);
            if (_hostarrayUInt32)
                memcpy(_hostarrayUInt32,
                       static_cast<const uint32_t*>(*_membufUInt32),
                       len * sizeof(uint32_t));
            }
            break;

        case (IBF) :
            {
            const size_t len = _imgbufFloat->width() * _imgbufFloat->height();
            fillconst<float>(static_cast<float*>(*_imgbufFloat), 1, len);
            if (_hostarrayFloat)
                memcpy(_hostarrayFloat,
                       static_cast<const float*>(*_imgbufFloat),
                       len * sizeof(float));
            }
            break;

        case (IBD) :
            {
            const size_t len = _imgbufDouble->width() * _imgbufDouble->height();
            fillconst<double>(static_cast<double*>(*_imgbufDouble), 1, len);
            if (_hostarrayDouble)
                memcpy(_hostarrayDouble,
                       static_cast<const double*>(*_imgbufDouble),
                       len * sizeof(double));
            }
            break;

        case (IBI) :
            {
            const size_t len = _imgbufInt32->width() * _imgbufInt32->height();
            fillconst<int32_t>(static_cast<int32_t*>(*_imgbufInt32), 1, len);
            if (_hostarrayInt32)
                memcpy(_hostarrayInt32,
                       static_cast<const int32_t*>(*_imgbufInt32),
                       len * sizeof(int32_t));
            }
            break;

        case (IBU) :
            {
            const size_t len = _imgbufUInt32->width() * _imgbufUInt32->height();
            fillconst<uint32_t>(static_cast<uint32_t*>(*_imgbufUInt32), 1, len);
            if (_hostarrayUInt32)
                memcpy(_hostarrayUInt32,
                       static_cast<const uint32_t*>(*_imgbufUInt32),
                       len * sizeof(uint32_t));
            }
            break;
    }
}

void ArrayBuf::fillRandom(void)
{
    switch (_devbuf)
    {
        case (MBF) :
            {
            const size_t len = _membufFloat->length();
            fillrand<float>(static_cast<float*>(*_membufFloat), len);
            if (_hostarrayFloat)
                memcpy(_hostarrayFloat,
                       static_cast<const float*>(*_membufFloat),
                       len * sizeof(float));
            }
            break;

        case (MBD) :
            {
            const size_t len = _membufDouble->length();
            fillrand<double>(static_cast<double*>(*_membufDouble), len);
            if (_hostarrayDouble)
                memcpy(_hostarrayDouble,
                       static_cast<const double*>(*_membufDouble),
                       len * sizeof(double));
            }
            break;

        case (MBI) :
            {
            const size_t len = _membufInt32->length();
            fillrand<int32_t>(static_cast<int32_t*>(*_membufInt32), len);
            if (_hostarrayInt32)
                memcpy(_hostarrayInt32,
                       static_cast<const int32_t*>(*_membufInt32),
                       len * sizeof(int32_t));
            }
            break;

        case (MBU) :
            {
            const size_t len = _membufUInt32->length();
            fillrand<uint32_t>(static_cast<uint32_t*>(*_membufUInt32), len);
            if (_hostarrayUInt32)
                memcpy(_hostarrayUInt32,
                       static_cast<const uint32_t*>(*_membufUInt32),
                       len * sizeof(uint32_t));
            }
            break;

        case (IBF) :
            {
            const size_t len = _imgbufFloat->width() * _imgbufFloat->height();
            fillrand<float>(static_cast<float*>(*_imgbufFloat), len);
            if (_hostarrayFloat)
                memcpy(_hostarrayFloat,
                       static_cast<const float*>(*_imgbufFloat),
                       len * sizeof(float));
            }
            break;

        case (IBD) :
            {
            const size_t len = _imgbufDouble->width() * _imgbufDouble->height();
            fillrand<double>(static_cast<double*>(*_imgbufDouble), len);
            if (_hostarrayDouble)
                memcpy(_hostarrayDouble,
                       static_cast<const double*>(*_imgbufDouble),
                       len * sizeof(double));
            }
            break;

        case (IBI) :
            {
            const size_t len = _imgbufInt32->width() * _imgbufInt32->height();
            fillrand<int32_t>(static_cast<int32_t*>(*_imgbufInt32), len);
            if (_hostarrayInt32)
                memcpy(_hostarrayInt32,
                       static_cast<const int32_t*>(*_imgbufInt32),
                       len * sizeof(int32_t));
            }
            break;

        case (IBU) :
            {
            const size_t len = _imgbufUInt32->width() * _imgbufUInt32->height();
            fillrand<uint32_t>(static_cast<uint32_t*>(*_imgbufUInt32), len);
            if (_hostarrayUInt32)
                memcpy(_hostarrayUInt32,
                       static_cast<const uint32_t*>(*_imgbufUInt32),
                       len * sizeof(uint32_t));
            }
            break;
    }
}

bool ArrayBuf::flushToDevice(void)
{
    switch (_devbuf)
    {
        case (MBF) : _cdev << *_membufFloat << FLUSH; break;
        case (MBD) : _cdev << *_membufDouble << FLUSH; break;
        case (MBI) : _cdev << *_membufInt32 << FLUSH; break;
        case (MBU) : _cdev << *_membufUInt32 << FLUSH; break;
        case (IBF) : _cdev << *_imgbufFloat << FLUSH; break;
        case (IBD) : _cdev << *_imgbufDouble << FLUSH; break;
        case (IBI) : _cdev << *_imgbufInt32 << FLUSH; break;
        case (IBU) : _cdev << *_imgbufUInt32 << FLUSH; break;
    }

    return _cdev.statusOp();
}

bool ArrayBuf::flushFromDevice(void)
{
    switch (_devbuf)
    {
        case (MBF) : _cdev >> *_membufFloat >> FLUSH; break;
        case (MBD) : _cdev >> *_membufDouble >> FLUSH; break;
        case (MBI) : _cdev >> *_membufInt32 >> FLUSH; break;
        case (MBU) : _cdev >> *_membufUInt32 >> FLUSH; break;
        case (IBF) : _cdev >> *_imgbufFloat >> FLUSH; break;
        case (IBD) : _cdev >> *_imgbufDouble >> FLUSH; break;
        case (IBI) : _cdev >> *_imgbufInt32 >> FLUSH; break;
        case (IBU) : _cdev >> *_imgbufUInt32 >> FLUSH; break;
    }

    return _cdev.statusOp();
}

bool ArrayBuf::argumentToKernel(OCLkernel& ckernel)
{
    switch (_devbuf)
    {
        case (MBF) : ckernel << *_membufFloat; break;
        case (MBD) : ckernel << *_membufDouble; break;
        case (MBI) : ckernel << *_membufInt32; break;
        case (MBU) : ckernel << *_membufUInt32; break;
        case (IBF) : ckernel << *_imgbufFloat; break;
        case (IBD) : ckernel << *_imgbufDouble; break;
        case (IBI) : ckernel << *_imgbufInt32; break;
        case (IBU) : ckernel << *_imgbufUInt32; break;
    }

    return ckernel.statusOp();
}

}; // namespace chai_internal
