// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifdef __LOGGING_ENABLED__
#include <sstream>
#endif

#include "Logger.hpp"
#include "MemManager.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

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

    ArrayBuf* arrayBuf = NULL;

    switch (backMem->precision())
    {
        case (PrecType::UInt32) :
            arrayBuf = createArrayBuf(
                           mode,
                           backMem->packing(),
                           backMem->W(),
                           backMem->H(),
                           vectorLength,
                           static_cast< uint32_t* >(backMem->ptrMem()) );
            break;

        case (PrecType::Int32) :
            arrayBuf = createArrayBuf(
                           mode,
                           backMem->packing(),
                           backMem->W(),
                           backMem->H(),
                           vectorLength,
                           static_cast< int32_t* >(backMem->ptrMem()) );
            break;

        case (PrecType::Float) :
            arrayBuf = createArrayBuf(
                           mode,
                           backMem->packing(),
                           backMem->W(),
                           backMem->H(),
                           vectorLength,
                           static_cast< float* >(backMem->ptrMem()) );
            break;

        case (PrecType::Double) :
            arrayBuf = createArrayBuf(
                           mode,
                           backMem->packing(),
                           backMem->W(),
                           backMem->H(),
                           vectorLength,
                           static_cast< double* >(backMem->ptrMem()) );
            break;
    }

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
                               0,
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
                               PrecType::Float,
                               vectorLength,
                               vt,
                               true,
                               val32,
                               0,
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
                                 const double val64)
{
    return createArrayInternal(varNum,
                               mode,
                               packing,
                               W,
                               H,
                               PrecType::Double,
                               vectorLength,
                               vt,
                               true,
                               0,
                               val64,
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
                                 const int32_t val32i)
{
    return createArrayInternal(varNum,
                               mode,
                               packing,
                               W,
                               H,
                               PrecType::Int32,
                               vectorLength,
                               vt,
                               true,
                               0,
                               0,
                               val32i,
                               0);
}

bool MemManager::createArrayTemp(const uint32_t varNum,
                                 const AccessMode mode,
                                 const size_t packing,
                                 const size_t W,
                                 const size_t H,
                                 const size_t vectorLength,
                                 VectorTrace& vt,
                                 const uint32_t val32u)
{
    return createArrayInternal(varNum,
                               mode,
                               packing,
                               W,
                               H,
                               PrecType::UInt32,
                               vectorLength,
                               vt,
                               true,
                               0,
                               0,
                               0,
                               val32u);
}

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

    ArrayBuf* arrayBuf = NULL;

    switch (backMem->precision())
    {
        case (PrecType::UInt32) :
            arrayBuf = createArrayBuf(
                           mode,
                           backMem->packing(),
                           backMem->W(),
                           backMem->H(),
                           vectorLength,
                           static_cast< uint32_t* >(backMem->ptrMem()) );
            break;

        case (PrecType::Int32) :
            arrayBuf = createArrayBuf(
                           mode,
                           backMem->packing(),
                           backMem->W(),
                           backMem->H(),
                           vectorLength,
                           static_cast< int32_t* >(backMem->ptrMem()) );
            break;

        case (PrecType::Float) :
            arrayBuf = createArrayBuf(
                           mode,
                           backMem->packing(),
                           backMem->W(),
                           backMem->H(),
                           vectorLength,
                           static_cast< float* >(backMem->ptrMem()) );
            break;

        case (PrecType::Double) :
            arrayBuf = createArrayBuf(
                           mode,
                           backMem->packing(),
                           backMem->W(),
                           backMem->H(),
                           vectorLength,
                           static_cast< double* >(backMem->ptrMem()) );
            break;
    }

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
                               0,
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
                               PrecType::Float,
                               vectorLength,
                               vt,
                               true,
                               val32,
                               0,
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
                                 const double val64)
{
    return createArrayInternal(varPtr,
                               mode,
                               packing,
                               W,
                               H,
                               PrecType::Double,
                               vectorLength,
                               vt,
                               true,
                               0,
                               val64,
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
                                 const int32_t val32i)
{
    return createArrayInternal(varPtr,
                               mode,
                               packing,
                               W,
                               H,
                               PrecType::Int32,
                               vectorLength,
                               vt,
                               true,
                               0,
                               0,
                               val32i,
                               0);
}

bool MemManager::createArrayTemp(const AstVariable* varPtr,
                                 const AccessMode mode,
                                 const size_t packing,
                                 const size_t W,
                                 const size_t H,
                                 const size_t vectorLength,
                                 VectorTrace& vt,
                                 const uint32_t val32u)
{
    return createArrayInternal(varPtr,
                               mode,
                               packing,
                               W,
                               H,
                               PrecType::UInt32,
                               vectorLength,
                               vt,
                               true,
                               0,
                               0,
                               0,
                               val32u);
}

}; // namespace chai_internal
