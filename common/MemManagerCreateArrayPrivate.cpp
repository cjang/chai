// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include "Logger.hpp"
#include "MemManager.hpp"
#include "PrecType.hpp"

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

ArrayBuf* MemManager::createArrayBuf(const AccessMode mode,
                                     const size_t packing,
                                     const size_t W,
                                     const size_t H,
                                     const size_t vectorLength,
                                     int32_t* ptr)
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
                                     uint32_t* ptr)
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
                                     const double val64,
                                     const int32_t val32i,
                                     const uint32_t val32u)
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

        // memalloc only allocates backing memory for make and read
        // zeros, ones and scalar literals do not have backing memory yet
        const size_t stmtIndex = vt.varFront(varNum);
        BackMem* backMem
            = vt.backMem().count( stmtIndex )
                  ? vt.backMem()[ stmtIndex ]
                  : allocBackMem(W, H, precision, vt.numTraces()); // allocate!

        // initialize backing memory
        if (initBackMem)
        {
            switch (precision)
            {
                case (PrecType::UInt32) :
                    for (size_t i = 0; i < backMem->sizeElems(); i++)
                        static_cast< uint32_t* >(backMem->ptrMem())[i] = val32u;
                    break;

                case (PrecType::Int32) :
                    for (size_t i = 0; i < backMem->sizeElems(); i++)
                        static_cast< int32_t* >(backMem->ptrMem())[i] = val32i;
                    break;

                case (PrecType::Float) :
                    for (size_t i = 0; i < backMem->sizeElems(); i++)
                        static_cast< float* >(backMem->ptrMem())[i] = val32;
                    break;

                case (PrecType::Double) :
                    for (size_t i = 0; i < backMem->sizeElems(); i++)
                        static_cast< double* >(backMem->ptrMem())[i] = val64;
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
                frontMem.push_back(allocFrontMem(W, H, precision, backMem, i));
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
                                     const double val64,
                                     const int32_t val32i,
                                     const uint32_t val32u)
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
                case (PrecType::UInt32) :
                    arrayBuf->fillValue(val32u);
                    break;

                case (PrecType::Int32) :
                    arrayBuf->fillValue(val32i);
                    break;

                case (PrecType::Float) :
                    arrayBuf->fillValue(val32);
                    break;

                case (PrecType::Double) :
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

}; // namespace chai_internal
