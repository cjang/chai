// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "MemTrans.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

ArrayBuf::AccessMode MemTrans::convertMode(const AccessMode mode) const
{
    switch (mode)
    {
        case (READ) : return ArrayBuf::READ;
        case (WRITE) : return ArrayBuf::WRITE;
        case (READWRITE) : return ArrayBuf::READWRITE;
    }
}

ArrayBuf* MemTrans::createArrayBuf(const AccessMode mode,
                                   const size_t PREC,
                                   const size_t vecLen,
                                   const size_t W,
                                   const size_t H,
                                   const size_t slots)
{
    ArrayBuf* arrayBuf = new ArrayBuf( *_oclDevice,
                                       convertMode(mode),
                                       PREC,
                                       vecLen,
                                       W,
                                       H,
                                       slots );

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

bool MemTrans::createArrayInternal(const uint32_t varNum,
                                   const AccessMode mode,
                                   const size_t PREC,
                                   const size_t vecLen,
                                   const size_t W,
                                   const size_t H,
                                   const size_t slots,
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
                return true;
            }
        }

        // memalloc only allocates backing memory for make and read
        // zeros, ones and scalar literals do not have backing memory yet
        const size_t stmtIndex = vt.varFront(varNum);
        BackMem* backMem
            = vt.backMem().count( stmtIndex )
                  ? vt.backMem()[ stmtIndex ]
                  : allocBackMem(PREC, W, H, vt.numTraces()); // allocate!

        // initialize backing memory
        if (initBackMem)
        {
            switch (PREC)
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
                frontMem.push_back(allocFrontMem(PREC, W, H, backMem, i));
            }

            // there must be nut objects for a live variable in the trace
            vt.vectorNuts()[ varNum ]->setNutMem(version, frontMem);
        }

        return createArraySendData(varNum,
                                   mode,
                                   vecLen,
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
                                             PREC,
                                             vecLen,
                                             W,
                                             H,
                                             slots );

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

bool MemTrans::createArrayInternal(const AstVariable* varPtr,
                                   const AccessMode mode,
                                   const size_t PREC,
                                   const size_t vecLen,
                                   const size_t W,
                                   const size_t H,
                                   const size_t slots,
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
                                         PREC,
                                         vecLen,
                                         W,
                                         H,
                                         slots );

    if (NULL != arrayBuf)
    {
        // initialize array memory
        if (initBackMem)
        {
            switch (PREC)
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
