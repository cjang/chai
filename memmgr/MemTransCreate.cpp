// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "MemTrans.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

bool MemTrans::createArraySendData(const uint32_t varNum,
                                   const AccessMode mode,
                                   const size_t vecLen,
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

    switch (backMem->prec())
    {
        case (PrecType::UInt32) :
            arrayBuf = createArrayBuf(
                           mode,
                           vecLen,
                           backMem->W(),
                           backMem->H(),
                           backMem->slots(),
                           static_cast< uint32_t* >(backMem->ptrMem()) );
            break;

        case (PrecType::Int32) :
            arrayBuf = createArrayBuf(
                           mode,
                           vecLen,
                           backMem->W(),
                           backMem->H(),
                           backMem->slots(),
                           static_cast< int32_t* >(backMem->ptrMem()) );
            break;

        case (PrecType::Float) :
            arrayBuf = createArrayBuf(
                           mode,
                           vecLen,
                           backMem->W(),
                           backMem->H(),
                           backMem->slots(),
                           static_cast< float* >(backMem->ptrMem()) );
            break;

        case (PrecType::Double) :
            arrayBuf = createArrayBuf(
                           mode,
                           vecLen,
                           backMem->W(),
                           backMem->H(),
                           backMem->slots(),
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

bool MemTrans::createArraySendData(const AstVariable* varPtr,
                                   const AccessMode mode,
                                   const size_t vecLen,
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

    switch (backMem->prec())
    {
        case (PrecType::UInt32) :
            arrayBuf = createArrayBuf(
                           mode,
                           vecLen,
                           backMem->W(),
                           backMem->H(),
                           backMem->slots(),
                           static_cast< uint32_t* >(backMem->ptrMem()) );
            break;

        case (PrecType::Int32) :
            arrayBuf = createArrayBuf(
                           mode,
                           vecLen,
                           backMem->W(),
                           backMem->H(),
                           backMem->slots(),
                           static_cast< int32_t* >(backMem->ptrMem()) );
            break;

        case (PrecType::Float) :
            arrayBuf = createArrayBuf(
                           mode,
                           vecLen,
                           backMem->W(),
                           backMem->H(),
                           backMem->slots(),
                           static_cast< float* >(backMem->ptrMem()) );
            break;

        case (PrecType::Double) :
            arrayBuf = createArrayBuf(
                           mode,
                           vecLen,
                           backMem->W(),
                           backMem->H(),
                           backMem->slots(),
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

}; // namespace chai_internal
