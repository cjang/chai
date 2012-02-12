// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>
#include <string.h>

#include "Logger.hpp"
#include "MemalignSTL.hpp"
#include "MemManager.hpp"

using namespace std;

namespace chai_internal {

ArrayBuf* MemManager::createArrayBuf(const AccessMode mode,
                                     const size_t packing,
                                     const size_t W,
                                     const size_t H,
                                     const size_t precision,
                                     const size_t vectorLength)
{
    ArrayBuf* arrayBuf = NULL;

    switch (mode)
    {
        case (READ) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::READ,
                                     packing,
                                     W,
                                     H,
                                     precision,
                                     vectorLength );
            break;

        case (WRITE) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::WRITE,
                                     packing,
                                     W,
                                     H,
                                     precision,
                                     vectorLength );
            break;

        case (READWRITE) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::READWRITE,
                                     packing,
                                     W,
                                     H,
                                     precision,
                                     vectorLength );
            break;
    }

    if (arrayBuf->bufOk())
    {
        return arrayBuf;
    }
    else
    {
        delete arrayBuf;
        return NULL;
    }
}

ArrayBuf* MemManager::createArrayBuf(const AccessMode mode,
                                     const size_t packing,
                                     const size_t W,
                                     const size_t H,
                                     const size_t vectorLength,
                                     float* ptr)
{
    ArrayBuf* arrayBuf = NULL;

    switch (mode)
    {
        case (READ) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::READ,
                                     packing,
                                     W,
                                     H,
                                     ptr,
                                     vectorLength );
            break;

        case (WRITE) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::WRITE,
                                     packing,
                                     W,
                                     H,
                                     ptr,
                                     vectorLength );
            break;

        case (READWRITE) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::READWRITE,
                                     packing,
                                     W,
                                     H,
                                     ptr,
                                     vectorLength );
            break;
    }

    if (arrayBuf->bufOk())
    {
        return arrayBuf;
    }
    else
    {
        delete arrayBuf;
        return NULL;
    }
}

ArrayBuf* MemManager::createArrayBuf(const AccessMode mode,
                                     const size_t packing,
                                     const size_t W,
                                     const size_t H,
                                     const size_t vectorLength,
                                     double* ptr)
{
    ArrayBuf* arrayBuf = NULL;

    switch (mode)
    {
        case (READ) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::READ,
                                     packing,
                                     W,
                                     H,
                                     ptr,
                                     vectorLength );
            break;

        case (WRITE) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::WRITE,
                                     packing,
                                     W,
                                     H,
                                     ptr,
                                     vectorLength );
            break;

        case (READWRITE) :
            arrayBuf = new ArrayBuf( *_oclDevice,
                                     ArrayBuf::READWRITE,
                                     packing,
                                     W,
                                     H,
                                     ptr,
                                     vectorLength );
            break;
    }

    if (arrayBuf->bufOk())
    {
        return arrayBuf;
    }
    else
    {
        delete arrayBuf;
        return NULL;
    }
}

bool MemManager::createArrayInternal(const uint32_t varNum,
                                     const AccessMode mode,
                                     const size_t packing,
                                     const size_t W,
                                     const size_t H,
                                     const size_t precision,
                                     const size_t vectorLength,
                                     VectorTrace& vt,
                                     const bool initBackMem,
                                     const float val32,
                                     const double val64)
{
    // need backing memory if variable is live as it may be subsequently
    // referenced within traces to swizzle data back from the compute device
    if ( vt.liveVariables().count(varNum) )
    {
        // variable is live

        // continuation check for pre-existing allocation
        for (vector< uint64_t >::const_iterator
             it = vt.hashCodeHistory().begin();
             it != vt.hashCodeHistory().end();
             it++)
        {
            const uint64_t hashCode = *it;

            // not locking as there is no mutation, hopefully this is safe
            if ( _liveArray.count(hashCode) &&
                 _liveArray[hashCode].count(varNum) )
            {
                // something already allocated
#ifdef __LOGGING_ENABLED__
                stringstream ss;
                ss << "something already allocated for " << varNum;
                LOGGER(ss.str())
#endif

                return true;
            }
        }

        const bool isDP = precision == sizeof(double);

        // memalloc only allocates backing memory for make and read
        // zeros, ones and scalar literals do not have backing memory yet
        BackMem* backMem
            = vt.backMem().count( vt.varFront(varNum) )
                  ? vt.backMem()[ vt.varFront(varNum) ]
                  : allocBackMem(W, H, isDP, vt.numTraces()); // allocate!

        // initialize backing memory
        if (initBackMem)
        {
            switch (precision)
            {
                case (sizeof(float)) :
                    for (size_t i = 0; i < backMem->sizeElems(); i++)
                        static_cast< float* >( backMem->ptrMem() )[i] = val32;
                    break;

                case (sizeof(double)) :
                    for (size_t i = 0; i < backMem->sizeElems(); i++)
                        static_cast< double* >( backMem->ptrMem() )[i] = val64;
                    break;
            }
        }

        // check for pre-existing front memory
        const uint32_t version = 1;
        if (vt.vectorNuts()[ varNum ]->getNutMem(version).empty())
        {
            // create front memory
            vector< FrontMem* > frontMem;
            for (size_t i = 0; i < vt.numTraces(); i++)
            {
                // allocate!
                frontMem.push_back( allocFrontMem(W, H, isDP, backMem, i) );
            }

            // there must be nut objects for a live variable in the trace
            vt.vectorNuts()[ varNum ]->setNutMem(version, frontMem);
        }

        return createArraySendData(varNum,
                                   mode,
                                   vectorLength,
                                   vt,
                                   backMem);
    }
    else
    {
        // variable is dead, there's no need for backing memory

        // continuation check for pre-existing allocation
        for (vector< uint64_t >::const_iterator
             it = vt.hashCodeHistory().begin();
             it != vt.hashCodeHistory().end();
             it++)
        {
            const uint64_t hashCode = *it;

            if ( _deadArray.count(hashCode) &&
                 _deadArray[hashCode].count(varNum) )
            {
                // something already allocated
                return true;
            }
        }

        ArrayBuf* arrayBuf = createArrayBuf( mode,
                                             packing,
                                             W,
                                             H,
                                             precision,
                                             vectorLength );

        if (NULL != arrayBuf)
        {
            // dead variable, this is a temporary
            _deadArray[vt.hashCode()][varNum] = arrayBuf;
            arrayBuf->flushToDevice();
            return true;
        }
        else
        {
            // failed to allocate device memory
            return false;
        }
    }
}

bool MemManager::createArrayInternal(const AstVariable* varPtr,
                                     const AccessMode mode,
                                     const size_t packing,
                                     const size_t W,
                                     const size_t H,
                                     const size_t precision,
                                     const size_t vectorLength,
                                     VectorTrace& vt,
                                     const bool initBackMem,
                                     const float val32,
                                     const double val64)
{
    // continuation check for pre-existing allocation
    for (vector< uint64_t >::const_iterator
         it = vt.hashCodeHistory().begin();
         it != vt.hashCodeHistory().end();
         it++)
    {
        const uint64_t hashCode = *it;

        if ( _splitArray.count(hashCode) &&
             _splitArray[hashCode].count(varPtr) )
        {
            // something already allocated
            return true;
        }
    }

    ArrayBuf* arrayBuf = createArrayBuf( mode,
                                         packing,
                                         W,
                                         H,
                                         precision,
                                         vectorLength );

    if (NULL != arrayBuf)
    {
        // initialize array memory
        if (initBackMem)
        {
            switch (precision)
            {
                case (sizeof(float)) :
                    arrayBuf->fillValue(val32);
                    break;

                case (sizeof(double)) :
                    arrayBuf->fillValue(val64);
                    break;
            }
        }

        _splitArray[vt.hashCode()][varPtr] = arrayBuf;
        arrayBuf->flushToDevice();
        return true;
    }
    else
    {
        // failed to allocate device memory
        return false;
    }
}

////////////////////////////////////////
// construction and destruction

MemManager::MemManager(const size_t deviceCode)
    : _deviceCode(deviceCode),
      _deviceIndex(-1),
      _deviceIsCPU(true),
      _oclInit(NULL),
      _oclDevice(NULL),
      _cache(NULL)
{
    pthread_mutex_init(&_memMutex, NULL);
}

MemManager::MemManager(const size_t deviceCode,
                       const size_t deviceIndex,
                       OCLinit& o,
                       OCLdevice& d)
    : _deviceCode(deviceCode),
      _deviceIndex(deviceIndex),
      _deviceIsCPU(o.devices().isCPU(deviceIndex)),
      _oclInit(&o),
      _oclDevice(&d),
      _cache(new KernelCache(d))
{
    pthread_mutex_init(&_memMutex, NULL);
}

MemManager::~MemManager(void)
{
    pthread_mutex_destroy(&_memMutex);

    delete _cache;
}

size_t MemManager::getDeviceCode(void) const
{
    return _deviceCode;
}

size_t MemManager::getDeviceIndex(void) const
{
    return _deviceIndex;
}

OCLdevice& MemManager::getComputeDevice(void)
{
    return *_oclDevice;
}

////////////////////////////////////////
// used by scheduler
// (allocate backing memory)

bool MemManager::memalloc(VectorTrace& vt)
{
    // iterate over statements that have array memory boxes
    for (map< size_t, vector< FrontMem* > >::const_iterator
         it = vt.frontMem().begin();
         it != vt.frontMem().end();
         it++)
    {
        const FrontMem* proto = (*it).second.front();

        // The API allocates array memory boxes only.
        // If the first one is empty, then they all are.
        // Deferred allocation is all or nothing.
        if ( NULL != proto->ptrMem() )
            continue; // skip if memory already allocated

        map< void*, size_t > dataPtrMap;
        vector< size_t >     frontIndex;

        // A single vectorized statement is all make or all read boxes.
        // This is a side-effect of bytecode hashing and the API.
        // If one array memory box has a deferred pointer, then they all do.
        if (proto->dataPtr())
        {
            // must be make boxes

            // determine distinct data pointers for loop fusion
            for (vector< FrontMem* >::const_iterator
                 jt = (*it).second.begin();
                 jt != (*it).second.end();
                 jt++)
            {
                void* p = (*jt)->dataPtr();

                if (0 == dataPtrMap.count(p))
                {
                    const size_t idx = dataPtrMap.size();
                    dataPtrMap[p] = idx;
                }

                frontIndex.push_back(dataPtrMap[p]);
            }
        }
        else
        {
            // must be read boxes

            for (size_t i = 0; i < vt.numTraces(); i++)
            {
                frontIndex.push_back(i);
            }
        }

        const size_t numBackingSlots = dataPtrMap.empty()
                                           ? vt.numTraces()     // read boxes
                                           : dataPtrMap.size(); // make boxes

        // standard vector length is double2 and float4
        const size_t standardVectorLength = proto->isDouble() ? 2 : 4;

        // special case for scalar reductions
        // relies on implicit convention of array dimensions
        const bool specialCaseReadScalar
            = 1 == proto->H() && standardVectorLength == proto->W();

        // if front memory is not even multiple of standard vector length
        const bool specialCasePadNeeded
            = 0 != (proto->W() % standardVectorLength);
        const size_t specialCasePadWidth
            = standardVectorLength * (1 + (proto->W() / standardVectorLength));

        // width is forced to be a multiple of standard vector length
        const size_t frontWidth = specialCasePadNeeded
                                      ? specialCasePadWidth
                                      : proto->W();

        // size of memory for each front memory box
        const size_t frontSize
            = proto->H() * frontWidth * (proto->isDouble() ? sizeof(double)
                                                           : sizeof(float));

        BackMem* backMem = NULL;

        // continuation check for pre-existing allocation
        const uint32_t traceVar = proto->variable();
        if (-1 != traceVar && vt.liveVariables().count(traceVar))
        {
            for (vector< uint64_t >::const_iterator
                 jt = vt.hashCodeHistory().begin();
                 jt != vt.hashCodeHistory().end();
                 jt++)
            {
                const uint64_t hashCode = *jt;

                // not locking as there is no mutation, hopefully this is safe
                if (_liveArray.count(hashCode) &&
                    _liveArray[hashCode].count(traceVar))
                {
                    ArrayBuf* arrayBuf = _liveArray[hashCode][traceVar];

                    // get the back memory object
                    if (_arrayBack.count(arrayBuf))
                    {
                        backMem = _arrayBack[arrayBuf];
                    }
                }
            }
        }

        void* b = NULL;

        // allocate backing memory
        if (NULL == backMem)
        {
            if (0 == posix_memalign(&b,
                                    MEMORY_ALIGNMENT,
                                    frontSize * numBackingSlots))
            {
                // allows summing oversized buffer in read_scalar()
                if (specialCaseReadScalar)
                {
                    // initialize memory to zero
                    memset(b, 0, frontSize * numBackingSlots);
                }

                backMem = proto->isDouble()
                              ? new BackMem(frontWidth,
                                            proto->H(),
                                            numBackingSlots,
                                            vt.numTraces(), // front count
                                            static_cast<double*>(b),
                                            this)
                              : new BackMem(frontWidth,
                                            proto->H(),
                                            numBackingSlots,
                                            vt.numTraces(), // front count
                                            static_cast<float*>(b),
                                            this);
            }
            else
            {
                // posix_memalign failed
                return false;
            }
        }

#ifdef __LOGGING_ENABLED__
        stringstream ss;
        ss << "backMem " << backMem << " for trace variable " << traceVar;
        LOGGER(ss.str())
#endif

        // iterate over statement array memory boxes
        for (size_t i = 0; i < (*it).second.size(); i++)
        {
            (*it).second[i]->setBackMem(
                static_cast< char* >(b) + frontIndex[i] * frontSize,
                backMem);
        }

        // check if re-used old backing memory or allocated new object
        if (NULL != b)
        {
            // front memory owns the backing memory
            backMem->release();
        }

        // copy deferred data
        if (proto->dataPtr())
        {
            for (map< void*, size_t >::const_iterator
                 jt = dataPtrMap.begin();
                 jt != dataPtrMap.end();
                 jt++)
            {
                // Padding makes the front memory boxes larger than the
                // deferred data. Row-major memory layout is also an issue.
                if (specialCasePadNeeded)
                {
                    const size_t dataRowSize
                        = proto->W() * (proto->isDouble() ? sizeof(double)
                                                          : sizeof(float));

                    const size_t padRowSize
                        = frontWidth * (proto->isDouble() ? sizeof(double)
                                                          : sizeof(float));

                    for (size_t j = 0; j < proto->H(); j++)
                    {
                        memcpy(static_cast< char* >(b)
                                   + (*jt).second * frontSize
                                   + padRowSize * j,
                               static_cast< char* >((*jt).first)
                                   + dataRowSize * j,
                               dataRowSize);

                        memset(static_cast< char* >(b)
                                   + (*jt).second * frontSize
                                   + padRowSize * j
                                   + dataRowSize,
                               0,
                               padRowSize - dataRowSize);
                    }
                }
                else
                {
                    // no padding, simple memory copy
                    memcpy(static_cast< char* >(b)
                               + (*jt).second * frontSize,
                           (*jt).first,
                           frontSize);
                }
            }
        }

        const size_t stmtIndex = (*it).first;

        // set backing memory in vector trace
        vt.backMem()[stmtIndex] = backMem;

        // remember readout and makedata memalloc layout
        if (-1 != proto->variable())
        {
            vt.memallocFront(
                   proto->variable(),
                   frontSize,
                   frontIndex );
        }
    }

    // success
    return true;
}

void MemManager::memfree(VectorTrace& vt, const bool freeMem)
{
    const uint64_t hashCode = vt.hashCode();

    // if device failed, then free immediately
    if (freeMem)
    {
        // iterate over vector trace statements with allocated memory
        for (map< size_t, vector< FrontMem* > >::const_iterator
             it = vt.frontMem().begin();
             it != vt.frontMem().end();
             it++)
        {
            // clear memory in every trace, release reference counts
            // this will delete backing memory objects
            for (vector< FrontMem* >::const_iterator
                 jt = (*it).second.begin();
                 jt != (*it).second.end();
                 jt++)
            {
                (*jt)->clearBackMem();
            }

            const size_t stmtIndex = (*it).first;

            // pointer to the back memory object
            BackMem* backMem = vt.backMem()[stmtIndex];

            // clear backing parent mem in vector trace (not deleting it!)
            vt.backMem().erase(stmtIndex);
        }

        pthread_mutex_lock(&_memMutex);

        // clean up compute device memory for live trace variables
        if (_liveArray.count(hashCode))
        {
            for (map< uint32_t, ArrayBuf* >::const_iterator
                 it = _liveArray[hashCode].begin();
                 it != _liveArray[hashCode].end();
                 it++)
            {
                ArrayBuf* arrayBuf = (*it).second;

                if (_arrayBack.count(arrayBuf))
                {
                    BackMem* backMem = _arrayBack[arrayBuf];

                    _backArray.erase(backMem);
                    _arrayBack.erase(arrayBuf);
                    _arrayCount.erase(arrayBuf);
                }

                delete arrayBuf;
            }

            _liveArray.erase(hashCode);
        }

        pthread_mutex_unlock(&_memMutex);
    }

    // clean up compute device memory for dead trace variables
    if (_deadArray.count(hashCode))
    {
        for (map< uint32_t, ArrayBuf* >::const_iterator
             it = _deadArray[hashCode].begin();
             it != _deadArray[hashCode].end();
             it++)
        {
            delete (*it).second;
        }

        _deadArray.erase(hashCode);
    }

    // split variables persist for duration of scheduled kernels only
    if (_splitArray.count(hashCode))
    {
        for (map< const AstVariable*, ArrayBuf* >::const_iterator
             it = _splitArray[hashCode].begin();
             it != _splitArray[hashCode].end();
             it++)
        {
            delete (*it).second;
        }

        _splitArray.erase(hashCode);
    }
}

////////////////////////////////////////
// callbacks from BackMem

void MemManager::swizzle(BackMem* backMem)
{
#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "backMem=" << backMem;
    LOGGER(ss.str())
#endif

    // only need to move data if using an OpenCL compute device that is remote
    if (_deviceCode >= GPU_OPENCL && !_deviceIsCPU)
    {
        pthread_mutex_lock(&_memMutex);

        // is this object still managed?
        if ( _backArray.count(backMem) )
        {
#ifdef __LOGGING_ENABLED__
            LOGGER("object still managed, about to flush")
#endif

            // OpenCL enqueue memory read and wait here
            _backArray[backMem]->flushFromDevice();
        }

        pthread_mutex_unlock(&_memMutex);
    }
}

// called from the BackMem destructor
void MemManager::untrackMem(BackMem* backMem)
{
    pthread_mutex_lock(&_memMutex);

    if (_backArray.count(backMem))
    {
        ArrayBuf* arrayBuf = _backArray[backMem];

        if (0 == --_arrayCount[arrayBuf])
        {
            _backArray.erase(backMem);
            _arrayBack.erase(arrayBuf);
            _arrayCount.erase(arrayBuf);

            delete arrayBuf;

            map< uint64_t, uint32_t > eraseKeys;

            for (map< uint64_t, map< uint32_t, ArrayBuf* > >::const_iterator
                 it = _liveArray.begin();
                 it != _liveArray.end();
                 it++)
            {
                for (map< uint32_t, ArrayBuf* >::const_iterator
                     jt = (*it).second.begin();
                     jt != (*it).second.end();
                     jt++)
                {
                    if ((*jt).second == arrayBuf)
                        eraseKeys[(*it).first] = (*jt).first;
                }
            }

            for (map< uint64_t, uint32_t >::const_iterator
                 it = eraseKeys.begin();
                 it != eraseKeys.end();
                 it++)
            {
                _liveArray[(*it).first].erase((*it).second);

                if (_liveArray[(*it).first].empty())
                    _liveArray.erase((*it).first);
            }
        }
    }

    pthread_mutex_unlock(&_memMutex);
}

////////////////////////////////////////
// used by JIT dispatching

bool MemManager::checkSameDataAcrossTraces(
                     const vector< FrontMem* >& frontMem )
{
    if (frontMem.empty())
        return false;

    // Check for same data across all traces:
    // 1. all front objects have the same back object
    // 2. front and back objects are the same dimensions

    const FrontMem* proto = frontMem.front();

    set< BackMem* > backPtrs;
    for (vector< FrontMem* >::const_iterator
         it = frontMem.begin();
         it != frontMem.end();
         it++)
    {
        backPtrs.insert((*it)->getBackMem());
    }

    // standard vector length is double2 and float4
    const size_t standardVectorLength = proto->isDouble() ? 2 : 4;

    // special case for scalars
    // relies on implicit convention of array dimensions
    const bool specialCaseReadScalar
        = 1 == proto->H() && standardVectorLength == proto->W();

    // if front memory is not even multiple of standard vector length
    const bool specialCasePadNeeded
        = 0 != (proto->W() % standardVectorLength);
    const size_t specialCasePadWidth
        = standardVectorLength * (1 + (proto->W() / standardVectorLength));

    // width is forced to be a multiple of standard vector length
    const size_t frontWidth = specialCasePadNeeded
                                  ? specialCasePadWidth
                                  : proto->W();

    return 1 == backPtrs.size() &&
           1 == proto->getBackMem()->packing() &&
           frontWidth == proto->getBackMem()->W() &&
           proto->H() == proto->getBackMem()->H();
}

BackMem* MemManager::unifyBackMem(const uint32_t variable,
                                  const uint32_t version,
                                  vector< FrontMem* >& frontMem)
{
#ifdef __LOGGING_ENABLED__
    for (size_t i = 0; i < frontMem.size(); i++)
    {
        stringstream ss;
        ss << "frontMem[" << i << "]=" << frontMem[i];
        LOGGER(ss.str())
    }
#endif

    const FrontMem* proto = frontMem.front();

    BackMem* backMem = NULL;

    if (checkSameDataAcrossTraces(frontMem))
    {
        backMem = proto->getBackMem();

#ifdef __LOGGING_ENABLED__
        if (NULL == backMem->ptrMem())
            LOGGER("back mem has null ptr")
#endif
    }

    // standard vector length is double2 and float4
    const size_t standardVectorLength = proto->isDouble() ? 2 : 4;

    // special case for scalars
    // relies on implicit convention of array dimensions
    const bool specialCaseReadScalar
        = 1 == proto->H() && standardVectorLength == proto->W();

    // if front memory is not even multiple of standard vector length
    const bool specialCasePadNeeded
        = 0 != (proto->W() % standardVectorLength);
    const size_t specialCasePadWidth
        = standardVectorLength * (1 + (proto->W() / standardVectorLength));

    // width is forced to be a multiple of standard vector length
    const size_t frontWidth = specialCasePadNeeded
                                  ? specialCasePadWidth
                                  : proto->W();

    // size of memory for each front memory box
    const size_t frontSize
        = proto->H() * frontWidth * (proto->isDouble() ? sizeof(double)
                                                       : sizeof(float));

    void* b = NULL;

    // allocate backing memory
    if (NULL == backMem)
    {
        if (0 == posix_memalign(&b,
                                MEMORY_ALIGNMENT,
                                frontSize * frontMem.size()))
        {
            // allows summing oversized buffer in read_scalar()
            if (specialCaseReadScalar)
            {
                // initialize memory to zero
                memset(b, 0, frontSize * frontMem.size());
            }

            backMem = proto->isDouble()
                          ? new BackMem(frontWidth,
                                        proto->H(),
                                        frontMem.size(),
                                        frontMem.size(), // front count
                                        static_cast<double*>(b),
                                        this)
                          : new BackMem(frontWidth,
                                        proto->H(),
                                        frontMem.size(),
                                        frontMem.size(), // front count
                                        static_cast<float*>(b),
                                        this);
        }
        else
        {
            // posix_memalign failed
            return NULL;
        }
    }

#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "variable=" << variable
       << " version=" << version
       << " backMem=" << backMem;
    LOGGER(ss.str())
#endif

    for (size_t i = 0; i < frontMem.size(); i++)
    {
        // only need to copy if new backing memory allocated
        if (NULL != b)
        {
            // Padding makes the front memory boxes larger than the
            // deferred data. Row-major memory layout is also an issue.
            if (specialCasePadNeeded)
            {
                const size_t dataRowSize
                    = proto->W() * (proto->isDouble() ? sizeof(double)
                                                      : sizeof(float));

                const size_t padRowSize
                    = frontWidth * (proto->isDouble() ? sizeof(double)
                                                      : sizeof(float));

                for (size_t j = 0; j < proto->H(); j++)
                {
                    memcpy(static_cast< char* >(b)
                               + frontSize * i
                               + padRowSize * j,
                           static_cast< char* >(frontMem[i]->ptrMem())
                               + dataRowSize * j,
                           dataRowSize);

                    memset(static_cast< char* >(b)
                               + frontSize * i
                               + padRowSize * j
                               + dataRowSize,
                           0,
                           padRowSize - dataRowSize);
                }
            }
            else
            {
                // no padding, simple memory copy
                memcpy(static_cast< char* >(b) + frontSize * i,
                       frontMem[i]->ptrMem(),
                       frontSize);
            }
        }

        if (frontMem[i]->getBackMem() != backMem)
        {
            frontMem[i]->clearBackMem();
            frontMem[i]->setBackMem(
                             static_cast< char* >(backMem->ptrMem())
                                 + frontSize * i,
                             backMem );
        }
    }

    // check if re-used old backing memory or allocated new object
    if (NULL != b)
    {
        // front memory owns the backing memory
        backMem->release();
    }

    return backMem;
}

////////////////////////////////////////
// direct access to array buffer objects

ArrayBuf* MemManager::getArrayBuf(const uint32_t traceVar,
                                  VectorTrace& vt)
{
    // start with most recent hash code and work backwards in history
    for (vector< uint64_t >::const_reverse_iterator
         it = vt.hashCodeHistory().rbegin();
         it != vt.hashCodeHistory().rend();
         it++)
    {
        const uint64_t hashCode = *it;

        // not locking as there is no mutation, hopefully this is safe
        if (_liveArray.count(hashCode) &&
            _liveArray[hashCode].count(traceVar))
        {
            return _liveArray[hashCode][traceVar];
        }
        else if (_deadArray.count(hashCode) &&
                 _deadArray[hashCode].count(traceVar))
        {
            return _deadArray[hashCode][traceVar];
        }
    }

    // nothing found
    return NULL;
}

ArrayBuf* MemManager::getArrayBuf(const AstVariable* splitVar,
                                  VectorTrace& vt)
{
    // start with most recent hash code and work backwards in history
    for (vector< uint64_t >::const_reverse_iterator
         it = vt.hashCodeHistory().rbegin();
         it != vt.hashCodeHistory().rend();
         it++)
    {
        const uint64_t hashCode = *it;

        if (_splitArray.count(hashCode) &&
            _splitArray[hashCode].count(splitVar))
        {
            return _splitArray[hashCode][splitVar];
        }
    }

    return NULL;
}

////////////////////////////////////////
// create memory on compute device

bool MemManager::createArraySendData(const uint32_t varNum,
                                     const AccessMode mode,
                                     const size_t vectorLength,
                                     VectorTrace& vt,
                                     BackMem* backMem)
{
    // continuation check for pre-existing allocation
    for (vector< uint64_t >::const_iterator
         it = vt.hashCodeHistory().begin();
         it != vt.hashCodeHistory().end();
         it++)
    {
        const uint64_t hashCode = *it;

        if (vt.liveVariables().count(varNum))
        {
            // not locking as there is no mutation, hopefully this is safe
            if ( _liveArray.count(hashCode) &&
                 _liveArray[hashCode].count(varNum) )
            {
                // something already allocated
                return true;
            }
        }
        else
        {
            if ( _deadArray.count(hashCode) &&
                 _deadArray[hashCode].count(varNum) )
            {
                // something already allocated
                return true;
            }
        }
    }

    ArrayBuf* arrayBuf
        = backMem->isDouble()
              ? createArrayBuf( mode,
                                backMem->packing(),
                                backMem->W(),
                                backMem->H(),
                                vectorLength,
                                static_cast< double* >(backMem->ptrMem()) )
              : createArrayBuf( mode,
                                backMem->packing(),
                                backMem->W(),
                                backMem->H(),
                                vectorLength,
                                static_cast< float* >(backMem->ptrMem()) );

    if (NULL != arrayBuf)
    {
        const uint64_t hashCode = vt.hashCode();

        // remember the compute device memory objects for live variables
        if (vt.liveVariables().count(varNum))
        {
            pthread_mutex_lock(&_memMutex);

            _backArray[backMem] = arrayBuf;
            _arrayBack[arrayBuf] = backMem;
            _arrayCount[arrayBuf] = backMem->frontCount();

#ifdef __LOGGING_ENABLED__
            stringstream ss;
            ss << "variable=" << varNum
               << " _backArray[" << backMem << "]=" << arrayBuf;
            LOGGER(ss.str())
#endif

            // variable is live
            _liveArray[hashCode][varNum] = arrayBuf;

            pthread_mutex_unlock(&_memMutex);
        }
        else
        {
            // variable is dead
            _deadArray[hashCode][varNum] = arrayBuf;
        }
        arrayBuf->flushToDevice();
        return true;
    }
    else
    {
        // failed to allocate device memory
        return false;
    }
}

bool MemManager::createArrayTemp(const uint32_t varNum,
                                 const AccessMode mode,
                                 const size_t packing,
                                 const size_t W,
                                 const size_t H,
                                 const size_t precision,
                                 const size_t vectorLength,
                                 VectorTrace& vt)
{
    return createArrayInternal(varNum,
                               mode,
                               packing,
                               W,
                               H,
                               precision,
                               vectorLength,
                               vt,
                               false,
                               0,
                               0);
}

bool MemManager::createArrayTemp(const uint32_t varNum,
                                 const AccessMode mode,
                                 const size_t packing,
                                 const size_t W,
                                 const size_t H,
                                 const size_t vectorLength,
                                 VectorTrace& vt,
                                 const float val32)
{
    return createArrayInternal(varNum,
                               mode,
                               packing,
                               W,
                               H,
                               sizeof(float),
                               vectorLength,
                               vt,
                               true,
                               val32,
                               0);
}

bool MemManager::createArrayTemp(const uint32_t varNum,
                                 const AccessMode mode,
                                 const size_t packing,
                                 const size_t W,
                                 const size_t H,
                                 const size_t vectorLength,
                                 VectorTrace& vt,
                                 const double val64)
{
    return createArrayInternal(varNum,
                               mode,
                               packing,
                               W,
                               H,
                               sizeof(double),
                               vectorLength,
                               vt,
                               true,
                               0,
                               val64);
}

////////////////////////////////////////
// temporary memory on compute device

bool MemManager::createArraySendData(const AstVariable* varPtr,
                                     const AccessMode mode,
                                     const size_t vectorLength,
                                     VectorTrace& vt,
                                     BackMem* backMem)
{
    // continuation check for pre-existing allocation
    for (vector< uint64_t >::const_iterator
         it = vt.hashCodeHistory().begin();
         it != vt.hashCodeHistory().end();
         it++)
    {
        const uint64_t hashCode = *it;

        if ( _splitArray.count(hashCode) &&
             _splitArray[hashCode].count(varPtr) )
        {
            // something already allocated
            return true;
        }
    }

    ArrayBuf* arrayBuf
        = backMem->isDouble()
              ? createArrayBuf( mode,
                                backMem->packing(),
                                backMem->W(),
                                backMem->H(),
                                vectorLength,
                                static_cast< double* >(backMem->ptrMem()) )
              : createArrayBuf( mode,
                                backMem->packing(),
                                backMem->W(),
                                backMem->H(),
                                vectorLength,
                                static_cast< float* >(backMem->ptrMem()) );

    if (NULL != arrayBuf)
    {
        _splitArray[vt.hashCode()][varPtr] = arrayBuf;
        arrayBuf->flushToDevice();
        return true;
    }
    else
    {
        // failed to allocate device memory
        return false;
    }
}

bool MemManager::createArrayTemp(const AstVariable* varPtr,
                                 const AccessMode mode,
                                 const size_t packing,
                                 const size_t W,
                                 const size_t H,
                                 const size_t precision,
                                 const size_t vectorLength,
                                 VectorTrace& vt)
{
    return createArrayInternal(varPtr,
                               mode,
                               packing,
                               W,
                               H,
                               precision,
                               vectorLength,
                               vt,
                               false,
                               0,
                               0);
}

bool MemManager::createArrayTemp(const AstVariable* varPtr,
                                 const AccessMode mode,
                                 const size_t packing,
                                 const size_t W,
                                 const size_t H,
                                 const size_t vectorLength,
                                 VectorTrace& vt,
                                 const float val32)
{
    return createArrayInternal(varPtr,
                               mode,
                               packing,
                               W,
                               H,
                               sizeof(float),
                               vectorLength,
                               vt,
                               true,
                               val32,
                               0);
}

bool MemManager::createArrayTemp(const AstVariable* varPtr,
                                 const AccessMode mode,
                                 const size_t packing,
                                 const size_t W,
                                 const size_t H,
                                 const size_t vectorLength,
                                 VectorTrace& vt,
                                 const double val64)
{
    return createArrayInternal(varPtr,
                               mode,
                               packing,
                               W,
                               H,
                               sizeof(double),
                               vectorLength,
                               vt,
                               true,
                               0,
                               val64);
}

////////////////////////////////////////
// used by interpreter during
// operation dispatch

BackMem* MemManager::allocBackMem(const size_t W,
                                  const size_t H,
                                  const bool isDP,
                                  const size_t vecSize)
{
    // standard vector length is double2 and float4
    const size_t standardVectorLength = isDP ? 2 : 4;

    // special case for scalar reductions
    // relies on implicit convention of array dimensions
    const bool specialCaseReadScalar = 1 == H && standardVectorLength == W;

    // if front memory is not even multiple of standard vector length
    const bool specialCasePadNeeded = 0 != (W % standardVectorLength);
    const size_t specialCasePadWidth
        = standardVectorLength * (1 + (W / standardVectorLength));

    // width is forced to be a multiple of standard vector length
    const size_t frontWidth = specialCasePadNeeded ? specialCasePadWidth : W;

    // size of memory for each front memory box
    const size_t frontSize
        = H * frontWidth * (isDP ? sizeof(double) : sizeof(float));

    void* b = NULL;

    if (0 == posix_memalign(&b,
                            MEMORY_ALIGNMENT,
                            frontSize * vecSize))
    {
        // JIT memory buffers are padded so the interpreter must match
        if (specialCasePadNeeded)
        {
            // initialize memory to zero
            memset(b, 0, frontSize * vecSize);
        }

        BackMem* backMem = isDP ? new BackMem(W,
                                              H,
                                              vecSize,
                                              vecSize, // front count
                                              static_cast<double*>(b),
                                              this)
                                : new BackMem(W,
                                              H,
                                              vecSize,
                                              vecSize, // front count
                                              static_cast<float*>(b),
                                              this);

#ifdef __LOGGING_ENABLED__
        stringstream ss;
        ss << "backMem=" << backMem
           << " W=" << W
           << " H=" << H
           << " vecSize=" << vecSize
           << " isDP=" << isDP;
        LOGGER(ss.str())
#endif

        // front memory owns the backing memory
        backMem->release();

        return backMem;
    }
    else
    {
        return NULL;
    }
}

FrontMem* MemManager::allocFrontMem(const size_t W,
                                    const size_t H,
                                    const bool isDP,
                                    BackMem* backMem,
                                    const size_t vecIndex) const
{
    // standard vector length is double2 and float4
    const size_t standardVectorLength = isDP ? 2 : 4;

    // special case for scalar reductions
    // relies on implicit convention of array dimensions
    const bool specialCaseReadScalar = 1 == H && standardVectorLength == W;

    // if front memory is not even multiple of standard vector length
    const bool specialCasePadNeeded = 0 != (W % standardVectorLength);
    const size_t specialCasePadWidth
        = standardVectorLength * (1 + (W / standardVectorLength));

    // width is forced to be a multiple of standard vector length
    const size_t frontWidth = specialCasePadNeeded ? specialCasePadWidth : W;

    // size of memory for each front memory box
    const size_t frontSize
        = H * frontWidth * (isDP ? sizeof(double) : sizeof(float));

    FrontMem* frontMem;

    if (isDP)
        frontMem = new FrontMem(W, H, static_cast<double*>(NULL));
    else
        frontMem = new FrontMem(W, H, static_cast<float*>(NULL));

#ifdef __LOGGING_ENABLED__
    stringstream ss;
    ss << "frontMem=" << frontMem;
    LOGGER(ss.str())
#endif

    frontMem->setBackMem(
        static_cast< char* >(backMem->ptrMem()) + vecIndex * frontSize,
        backMem);

    return frontMem;
}

// enqueue kernel
bool MemManager::enqueueKernel(VectorTrace& vt,
                               Function& func,
                               const size_t globalWidth,
                               const size_t globalHeight)
{
    // check cache
    OCLkernel* kernelObj = _cache->factoryLookup(func);
    if (! kernelObj)
    {
        // failed, no kernel available
        return false;
    }

    // index space
    *kernelObj << OCLWorkIndex(globalWidth, 1);
    if (0 != globalHeight)
    {
        *kernelObj << OCLWorkIndex(globalHeight, 1);
    }

    // set kernel arguments
    for (vector< Variable* >::const_iterator
         it = func.arguments().begin();
         it != func.arguments().end();
         it++)
    {
        if (const AstVariable* astVar = func.splitVar(*it))
        {
            getArrayBuf(astVar, vt)->argumentToKernel(*kernelObj);
        }
        else
        {
            const uint32_t varNum = func.traceVar(*it);
            getArrayBuf(varNum, vt)->argumentToKernel(*kernelObj);
        }
    }

    // run kernel, note the flush triggers previous memory transfers too
    *_oclDevice << *kernelObj << FLUSH;

    // success
    return true;
}

}; // namespace chai_internal
