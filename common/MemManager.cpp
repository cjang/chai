// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>
#include <string.h>

#include "MemalignSTL.hpp"
#include "MemManager.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// construction and destruction

MemManager::MemManager(const size_t deviceCode)
    : _deviceCode(deviceCode),
      _oclInit(NULL),
      _oclDevice(NULL)
{
    pthread_mutex_init(&_memMutex, NULL);
}

MemManager::MemManager(const size_t deviceCode,
                       OCLinit& o,
                       OCLdevice& d)
    : _deviceCode(deviceCode),
      _oclInit(&o),
      _oclDevice(&d)
{
    pthread_mutex_init(&_memMutex, NULL);
}

MemManager::~MemManager(void)
{
    pthread_mutex_destroy(&_memMutex);
}

size_t MemManager::getDeviceCode(void) const
{
    return _deviceCode;
}

////////////////////////////////////////
// used by scheduler
// (allocate backing memory)

void MemManager::memalloc(VectorTrace& vt)
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
            // determine distinct data pointers for loop fusion
            for (size_t i = 0; i < vt.numTraces(); i++)
            {
                FrontMem* frontMem = (*it).second[i];

                void* p = frontMem->dataPtr();

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
            for (size_t i = 0; i < vt.numTraces(); i++)
            {
                frontIndex.push_back(i);
            }
        }

        const size_t numBackingSlots = dataPtrMap.empty()
                                           ? vt.numTraces()
                                           : dataPtrMap.size();

        const size_t frontSize = proto->sizeBytes();
        void* b                = NULL;

        // allocate backing memory
        if (0 == posix_memalign(&b,
                                MEMORY_ALIGNMENT,
                                frontSize * numBackingSlots))
        {
            BackMem* backMem
                = proto->isDouble()
                      ? new BackMem(proto->W(),
                                    proto->H() * numBackingSlots,
                                    static_cast<double*>(b),
                                    this)
                      : new BackMem(proto->W(),
                                    proto->H() * numBackingSlots,
                                    static_cast<float*>(b),
                                    this);

            // iterate over statement array memory boxes
            for (size_t i = 0; i < vt.numTraces(); i++)
            {
                FrontMem* frontMem = (*it).second[i];

                frontMem->setBackMem(
                    static_cast< char* >(b) + frontIndex[i] * frontSize,
                    backMem);
            }

            // front memory owns the backing memory
            backMem->release();

            // copy deferred data
            if (proto->dataPtr())
            {
                for (map< void*, size_t >::const_iterator
                     it = dataPtrMap.begin();
                     it != dataPtrMap.end();
                     it++)
                {
                    memcpy(static_cast< char* >(b) + (*it).second * frontSize,
                           (*it).first,
                           frontSize);
                }
            }

            const size_t stmtIndex = (*it).first;

            // set backing memory in vector trace
            vt.backMem()[stmtIndex] = backMem;
        }
        else
        {
            cerr << "FAILURE: posix_memalign "
                 << frontSize << " "
                 << vt.numTraces() << endl;
        }
    }
}

void MemManager::memfree(VectorTrace& vt, const bool freeMem)
{
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
            for (size_t i = 0; i < vt.numTraces(); i++)
            {
                (*it).second[i]->clearBackMem();
            }

            const size_t stmtIndex = (*it).first;

            // pointer to the back memory object
            BackMem* backMem = vt.backMem()[stmtIndex];

            // clear backing parent mem in vector trace (not deleting it!)
            vt.backMem().erase(stmtIndex);
        }

        // clean up compute device memory for live trace variables
        vector< uint32_t > liveVars;
        for (map< uint32_t, DeviceMem* >::const_iterator
             it = vt.traceMem().begin();
             it != vt.traceMem().end();
             it++)
        {
            const uint32_t variable = (*it).first;

            if ( vt.liveVariables().count(variable) )
            {
                DeviceMem* dm = (*it).second;

                pthread_mutex_lock(&_memMutex);

                if (_devBack.count(dm))
                {
                    BackMem* bm = _devBack[dm];

                    _backDev.erase(bm);
                    _devBack.erase(dm);

                    delete dm;

                    liveVars.push_back(variable);
                }

                pthread_mutex_unlock(&_memMutex);
            }
        }

        // erase any cleaned up compute device memory for live trace variables
        for (vector< uint32_t >::const_iterator
             it = liveVars.begin();
             it != liveVars.end();
             it++)
        {
            vt.traceMem().erase(*it);
        }
    }

    // clean up compute device memory for dead trace variables
    vector< uint32_t > deadVars;
    for (map< uint32_t, DeviceMem* >::const_iterator
         it = vt.traceMem().begin();
         it != vt.traceMem().end();
         it++)
    {
        if (0 == vt.liveVariables().count((*it).first))
        {
            delete (*it).second;
            deadVars.push_back((*it).first);
        }
    }
    for (vector< uint32_t >::const_iterator
         it = deadVars.begin();
         it != deadVars.end();
         it++)
    {
        vt.traceMem().erase(*it);
    }

    // split variables persist for duration of scheduled kernels only
    for (map< AstVariable*, DeviceMem* >::const_iterator
         it = vt.splitMem().begin();
         it != vt.splitMem().end();
         it++)
    {
        delete (*it).second;
    }
    vt.splitMem().clear();
}

////////////////////////////////////////
// callbacks from BackMem

void MemManager::swizzle(BackMem* bm)
{
    // only need to move data if using an OpenCL compute device
    if (_deviceCode >= GPU_OPENCL)
    {
        pthread_mutex_lock(&_memMutex);

        // is this object still managed?
        if ( _backDev.count(bm) )
        {
            // OpenCL enqueue memory read and wait here
            _backDev[bm]->extract(*_oclDevice);
        }

        pthread_mutex_unlock(&_memMutex);

        untrackMem(bm);
    }
}

// called from the BackMem destructor
void MemManager::untrackMem(BackMem* bm)
{
    pthread_mutex_lock(&_memMutex);

    if (_backDev.count(bm))
    {
        DeviceMem* dm = _backDev[bm];

        _backDev.erase(bm);
        _devBack.erase(dm);

        delete dm;
    }

    pthread_mutex_unlock(&_memMutex);
}

////////////////////////////////////////
// used by JIT dispatching

BackMem* MemManager::unifyBackMem(const uint32_t variable,
                                  const uint32_t version,
                                  vector< FrontMem* >& frontMem)
{
    const FrontMem* proto = frontMem.front();

    // Check for same data across all traces:
    // 1. all front objects have the same back object
    // 2. front and back objects are the same dimensions
    set< BackMem* > backPtrs;
    for (vector< FrontMem* >::const_iterator
         it = frontMem.begin();
         it != frontMem.end();
         it++)
    {
        backPtrs.insert((*it)->getBackMem());
    }
    if ( 1 == backPtrs.size() &&
         proto->W() == proto->getBackMem()->W() &&
         proto->H() == proto->getBackMem()->H() )
    {
        return proto->getBackMem();
    }

    void* b = NULL;

    // allocate backing memory
    if (0 == posix_memalign(&b,
                            MEMORY_ALIGNMENT,
                            frontMem.size() * proto->sizeBytes()))
    {
        BackMem* backMem
            = proto->isDouble()
                  ? new BackMem(proto->W(),
                                proto->H() * frontMem.size(),
                                static_cast<double*>(b),
                                this)
                  : new BackMem(proto->W(),
                                proto->H() * frontMem.size(),
                                static_cast<float*>(b),
                                this);

        for (size_t i = 0; i < frontMem.size(); i++)
        {
            memcpy(static_cast< char* >(b) + proto->sizeBytes() * i,
                   frontMem[i]->ptrMem(),
                   proto->sizeBytes());

            frontMem[i]->clearBackMem();
            frontMem[i]->setBackMem(
                             static_cast< char* >(b) + proto->sizeBytes() * i,
                             backMem );
        }

        return backMem;
    }
    else
    {
        cerr << "FAILURE: posix_memalign "
             << proto->sizeBytes() << " "
             << frontMem.size() << endl;

        return NULL;
    }
}

////////////////////////////////////////
// create memory on compute device

bool MemManager::createDevMem(VectorTrace& vt,
                              const uint32_t variableOnDevice,
                              BackMem* backMem,
                              const bool isReadTexture)
{
    // number of traces
    const size_t vecSize = vt.numTraces();

    const size_t W = backMem->W();
    const size_t H = backMem->H();

    DeviceMem* m;

    if (backMem->isDouble())
    {
        if (isReadTexture)
        {
            m = new DeviceImgbuf64(
                        *_oclDevice,
                        W,
                        H * vecSize,
                        static_cast< const double* >(backMem->ptrMem()) );
        }
        else
        {
            m = new DeviceMembuf64(
                        *_oclDevice,
                        W * H * vecSize,
                        static_cast< double* >(backMem->ptrMem()) );
        }
    }
    else
    {
        if (isReadTexture)
        {
            m = new DeviceImgbuf32(
                        *_oclDevice,
                        W,
                        H * vecSize,
                        static_cast< const float* >(backMem->ptrMem()) );
        }
        else
        {
            m = new DeviceMembuf32(
                        *_oclDevice,
                        W * H * vecSize,
                        static_cast< float* >(backMem->ptrMem()) );
        }
    }

    if (m->isOk())
    {
        // remember the compute device memory objects for live variables
        if (vt.liveVariables().count(variableOnDevice))
        {
            pthread_mutex_lock(&_memMutex);

            _backDev[backMem] = m;
            _devBack[m] = backMem;

            pthread_mutex_unlock(&_memMutex);
        }

        // variable may be live or dead
        vt.traceMem()[variableOnDevice] = m;
        m->insert(*_oclDevice);
        return true;
    }
    else
    {
        // failed to allocate device memory
        delete m;
        return false;
    }
}

bool MemManager::createDevMem(VectorTrace& vt,
                              const uint32_t variableOnDevice,
                              const size_t W,
                              const size_t H,
                              const bool isDP)
{
    // number of traces
    const size_t vecSize = vt.numTraces();

    // need backing memory if variable is live as it may be subsequently
    // referenced within traces to swizzle data back from the compute device
    if ( vt.liveVariables().count(variableOnDevice) )
    {
        // variable is live

        // create backing memory
        BackMem* backMem = allocBackMem(W, H, isDP, vecSize);

        // create front memory
        vector< FrontMem* > frontMem;
        for (size_t i = 0; i < vecSize; i++)
        {
            frontMem.push_back( allocFrontMem(W, H, isDP, backMem, i) );
        }

        // there must be nut objects for a live variable in the trace
        const uint32_t version = 1;
        vt.vectorNuts()[variableOnDevice]->setNut(version, frontMem);

        return createDevMem(vt, variableOnDevice, backMem, false);
    }
    else
    {
        // variable is dead, there's no need for backing memory

        DeviceMem* m;

        if (isDP)
        {
            m = new DeviceMembuf64(
                        *_oclDevice,
                        W * H * vecSize );
        }
        else
        {
            m = new DeviceMembuf32(
                        *_oclDevice,
                        W * H * vecSize );
        }

        if (m->isOk())
        {
            // dead variable, this is a temporary
            vt.traceMem()[variableOnDevice] = m;
            m->insert(*_oclDevice);
            return true;
        }
        else
        {
            // failed to allocate device memory
            delete m;
            return false;
        }
    }
}

bool MemManager::createDevMem(VectorTrace& vt,
                              const uint32_t variableOnDevice,
                              const size_t W,
                              const size_t H,
                              const float value)
{
    // number of traces
    const size_t vecSize = vt.numTraces();

    // need backing memory if variable is live as it may be subsequently
    // referenced within traces to swizzle data back from the compute device
    if ( vt.liveVariables().count(variableOnDevice) )
    {
        // variable is live

        // create backing memory
        BackMem* backMem = allocBackMem(W, H, false, vecSize);

        // initialize backing memory
        for (size_t i = 0; i < backMem->sizeElems(); i++)
        {
            static_cast< float* >( backMem->ptrMem() )[i] = value;
        }

        // create front memory
        vector< FrontMem* > frontMem;
        for (size_t i = 0; i < vecSize; i++)
        {
            frontMem.push_back( allocFrontMem(W, H, false, backMem, i) );
        }

        // there must be nut objects for a live variable in the trace
        const uint32_t version = 1;
        vt.vectorNuts()[variableOnDevice]->setNut(version, frontMem);

        return createDevMem(vt, variableOnDevice, backMem, false);
    }
    else
    {
        // variable is dead, there's no need for backing memory

        DeviceMembuf32* m = new DeviceMembuf32(
                                    *_oclDevice,
                                    W * H * vecSize );

        // initialize array memory
        for (size_t i = 0; i < W * H * vecSize; i++)
        {
            m->ptrMem()[ i ] = value;
        }

        if (m->isOk())
        {
            // dead variable, this is a temporary
            vt.traceMem()[variableOnDevice] = m;
            m->insert(*_oclDevice);
            return true;
        }
        else
        {
            // failed to allocate device memory
            delete m;
            return false;
        }
    }
}

bool MemManager::createDevMem(VectorTrace& vt,
                              const uint32_t variableOnDevice,
                              const size_t W,
                              const size_t H,
                              const double value)
{
    // number of traces
    const size_t vecSize = vt.numTraces();

    // need backing memory if variable is live as it may be subsequently
    // referenced within traces to swizzle data back from the compute device
    if ( vt.liveVariables().count(variableOnDevice) )
    {
        // variable is live

        // create backing memory
        BackMem* backMem = allocBackMem(W, H, true, vecSize);

        // initialize backing memory
        for (size_t i = 0; i < backMem->sizeElems(); i++)
        {
            static_cast< double* >( backMem->ptrMem() )[i] = value;
        }

        // create front memory
        vector< FrontMem* > frontMem;
        for (size_t i = 0; i < vecSize; i++)
        {
            frontMem.push_back( allocFrontMem(W, H, true, backMem, i) );
        }

        // there must be nut objects for a live variable in the trace
        const uint32_t version = 1;
        vt.vectorNuts()[variableOnDevice]->setNut(version, frontMem);

        return createDevMem(vt, variableOnDevice, backMem, false);
    }
    else
    {
        // variable is dead, there's no need for backing memory

        DeviceMembuf64* m = new DeviceMembuf64(
                                    *_oclDevice,
                                    W * H * vecSize );

        // initialize array memory
        for (size_t i = 0; i < W * H * vecSize; i++)
        {
            m->ptrMem()[ i ] = value;
        }

        if (m->isOk())
        {
            // dead variable, this is a temporary
            vt.traceMem()[variableOnDevice] = m;
            m->insert(*_oclDevice);
            return true;
        }
        else
        {
            // failed to allocate device memory
            delete m;
            return false;
        }
    }
}

////////////////////////////////////////
// temporary memory on compute device

bool MemManager::createDevMem(VectorTrace& vt,
                              AstVariable* variableOnDevice,
                              BackMem* backMem,
                              const bool isReadTexture)
{
    // number of traces
    const size_t vecSize = vt.numTraces();

    const size_t W = backMem->W();
    const size_t H = backMem->H();

    DeviceMem* m;

    if (backMem->isDouble())
    {
        if (isReadTexture)
        {
            m = new DeviceImgbuf64(
                        *_oclDevice,
                        W,
                        H * vecSize,
                        static_cast< const double* >(backMem->ptrMem()) );
        }
        else
        {
            m = new DeviceMembuf64(
                        *_oclDevice,
                        W * H * vecSize,
                        static_cast< double* >(backMem->ptrMem()) );
        }
    }
    else
    {
        if (isReadTexture)
        {
            m = new DeviceImgbuf32(
                        *_oclDevice,
                        W,
                        H * vecSize,
                        static_cast< const float* >(backMem->ptrMem()) );
        }
        else
        {
            m = new DeviceMembuf32(
                        *_oclDevice,
                        W * H * vecSize,
                        static_cast< float* >(backMem->ptrMem()) );
        }
    }

    if (m->isOk())
    {
        vt.splitMem()[variableOnDevice] = m;
        m->insert(*_oclDevice);
        return true;
    }
    else
    {
        // failed to allocate device memory
        delete m;
        return false;
    }
}

bool MemManager::createDevMem(VectorTrace& vt,
                              AstVariable* variableOnDevice,
                              const size_t W,
                              const size_t H,
                              const bool isDP)
{
    // number of traces
    const size_t vecSize = vt.numTraces();

    DeviceMem* m;

    if (isDP)
    {
        m = new DeviceMembuf64(
                    *_oclDevice,
                    W * H * vecSize );
    }
    else
    {
        m = new DeviceMembuf32(
                    *_oclDevice,
                    W * H * vecSize );
    }

    if (m->isOk())
    {
        vt.splitMem()[variableOnDevice] = m;
        m->insert(*_oclDevice);
        return true;
    }
    else
    {
        // failed to allocate device memory
        delete m;
        return false;
    }
}

bool MemManager::createDevMem(VectorTrace& vt,
                              AstVariable* variableOnDevice,
                              const size_t W,
                              const size_t H,
                              const float value)
{
    // number of traces
    const size_t vecSize = vt.numTraces();

    DeviceMembuf32* m = new DeviceMembuf32(
                                *_oclDevice,
                                W * H * vecSize );

    // initialize array memory
    for (size_t i = 0; i < W * H * vecSize; i++)
    {
        m->ptrMem()[ i ] = value;
    }

    if (m->isOk())
    {
        vt.splitMem()[variableOnDevice] = m;
        m->insert(*_oclDevice);
        return true;
    }
    else
    {
        // failed to allocate device memory
        delete m;
        return false;
    }
}

bool MemManager::createDevMem(VectorTrace& vt,
                              AstVariable* variableOnDevice,
                              const size_t W,
                              const size_t H,
                              const double value)
{
    // number of traces
    const size_t vecSize = vt.numTraces();

    DeviceMembuf64* m = new DeviceMembuf64(
                                *_oclDevice,
                                W * H * vecSize );

    // initialize array memory
    for (size_t i = 0; i < W * H * vecSize; i++)
    {
        m->ptrMem()[ i ] = value;
    }

    if (m->isOk())
    {
        vt.splitMem()[variableOnDevice] = m;
        m->insert(*_oclDevice);
        return true;
    }
    else
    {
        // failed to allocate device memory
        delete m;
        return false;
    }
}

////////////////////////////////////////
// used by interpreter during
// operation dispatch

BackMem* MemManager::allocBackMem(const size_t W,
                                  const size_t H,
                                  const bool isDP,
                                  const size_t vecSize)
{
    void* b = NULL;

    const size_t frontSize = W * H * (isDP ? sizeof(double) : sizeof(float));

    if (0 == posix_memalign(&b,
                            MEMORY_ALIGNMENT,
                            frontSize * vecSize))
    {
        BackMem* backMem = isDP ? new BackMem(W,
                                              H * vecSize,
                                              static_cast<double*>(b),
                                              this)
                                : new BackMem(W,
                                              H * vecSize,
                                              static_cast<float*>(b),
                                              this);

        // front memory owns the backing memory
        backMem->release();

        return backMem;
    }
    else
    {
        cerr << "FAILURE: posix_memalign "
             << frontSize << " "
             << vecSize << endl;

        return NULL;
    }
}

FrontMem* MemManager::allocFrontMem(const size_t W,
                                    const size_t H,
                                    const bool isDP,
                                    BackMem* backMem,
                                    const size_t vecIndex) const
{
    FrontMem* frontMem;

    if (isDP)
    {
        frontMem = new FrontMem(W, H, static_cast<double*>(NULL));

        frontMem->setBackMem(
            static_cast<double*>(backMem->ptrMem()) + vecIndex * W * H,
            backMem);
    }
    else
    {
        frontMem = new FrontMem(W, H, static_cast<float*>(NULL));

        frontMem->setBackMem(
            static_cast<float*>(backMem->ptrMem()) + vecIndex * W * H,
            backMem);
    }

    return frontMem;
}

}; // namespace chai_internal
