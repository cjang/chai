// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpMatmulBase.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// matmul

#define VECVEC_GENERAL \
switch (PREC) \
{ \
    case (PrecType::UInt32) : \
        addgen(tmpAB, uintPtr(2, i), W(1), W(0), m->uintPtr()); break; \
    case (PrecType::Int32) : \
        addgen(tmpAB, intPtr(2, i), W(1), W(0), m->intPtr()); break; \
    case (PrecType::Float) : \
        addgen(tmpAB, floatPtr(2, i), W(1), W(0), m->floatPtr()); break; \
    case (PrecType::Double) : \
        addgen(tmpAB, doublePtr(2, i), W(1), W(0), m->doublePtr()); break; \
}

#define VECMAT_GENERAL \
switch (PREC) \
{ \
    case (PrecType::UInt32) : \
        addgen(tmpAB, uintPtr(2, i), W(1), 1, m->uintPtr()); break; \
    case (PrecType::Int32) : \
        addgen(tmpAB, intPtr(2, i), W(1), 1, m->intPtr()); break; \
    case (PrecType::Float) : \
        addgen(tmpAB, floatPtr(2, i), W(1), 1, m->floatPtr()); break; \
    case (PrecType::Double) : \
        addgen(tmpAB, doublePtr(2, i), W(1), 1, m->doublePtr()); break; \
}

#define MATVEC_GENERAL \
switch (PREC) \
{ \
    case (PrecType::UInt32) : \
        addgen(tmpAB, uintPtr(2, i), H(0), 1, m->uintPtr()); break; \
    case (PrecType::Int32) : \
        addgen(tmpAB, intPtr(2, i), H(0), 1, m->uintPtr()); break; \
    case (PrecType::Float) : \
        addgen(tmpAB, floatPtr(2, i), H(0), 1, m->floatPtr()); break; \
    case (PrecType::Double) : \
        addgen(tmpAB, doublePtr(2, i), H(0), 1, m->doublePtr()); break; \
}

#define MATMAT_GENERAL \
switch (PREC) \
{ \
    case (PrecType::UInt32) : \
        addgen(tmpAB, uintPtr(2, i), W(1), H(0), m->uintPtr()); break; \
    case (PrecType::Int32) : \
        addgen(tmpAB, intPtr(2, i), W(1), H(0), m->intPtr()); break; \
    case (PrecType::Float) : \
        addgen(tmpAB, floatPtr(2, i), W(1), H(0), m->floatPtr()); break; \
    case (PrecType::Double) : \
        addgen(tmpAB, doublePtr(2, i), W(1), H(0), m->doublePtr()); break; \
}

void InterpMatmulBase::doMatmul(stack< vector< FrontMem* > >& outStack)
{
    const bool isGeneral = 2 == _inCount &&  // alpha, beta
                           3 == _outCount;   // A, B, C

    swizzle(0);
    swizzle(1);
    if (isGeneral) swizzle(2);

    const size_t prec0 = prec(0);
    const size_t prec1 = prec(1);
    const size_t PREC  = isGeneral
                             ? prec(2)
                             : max<size_t>(prec0, prec1);

    size_t WIDTH, HEIGHT, LEN;
    if ( (1 == H(0)) && (1 == H(1)) )  // vector * vector
    {
        WIDTH  = isGeneral ? W(2) : W(1);
        HEIGHT = isGeneral ? H(2) : W(0);
        LEN    = W(1) * W(0);
    }
    else if (1 == H(0))                // vector * matrix
    {
        WIDTH  = isGeneral ? W(2) : W(1);
        HEIGHT = isGeneral ? H(2) : 1;
        LEN    = W(1);
    }
    else if (1 == H(1))                // matrix * vector
    {
        WIDTH  = isGeneral ? W(2) : H(0);
        HEIGHT = isGeneral ? H(2) : 1;
        LEN    = H(0);
    }
    else                               // matrix * matrix
    {
        WIDTH  = isGeneral ? W(2) : W(1);
        HEIGHT = isGeneral ? H(2) : H(0);
        LEN    = W(1) * H(0);
    }

    const size_t SLOTS = isGeneral
                             ? slots(2)
                             : max<size_t>(slots(0), slots(1));

    BackMem* backMem;

    // array memory boxes
    vector< FrontMem* > frontMem;

    if ( (1 == H(0)) && (1 == H(1)) )  // vector * vector
    {
        // first allocate backing memory
        backMem = allocBackMem(PREC, WIDTH, HEIGHT, SLOTS);

        // calculate and create fronts
        for (size_t i = 0; i < SLOTS; i++)
        {
            FrontMem* m = allocFrontMem(PREC, WIDTH, HEIGHT, backMem, i);

            frontMem.push_back(m);

            switch (prec0) {
            case (PrecType::UInt32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        uint32_t tmpAB[LEN];
                        vecvec(uintPtr(0, i), uintPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(uintPtr(0, i), uintPtr(1, i),
                               m->uintPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        vecvec(uintPtr(0, i), intPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(uintPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecvec(uintPtr(0, i), floatPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(uintPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecvec(uintPtr(0, i), doublePtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(uintPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        vecvec(intPtr(0, i), uintPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(intPtr(0, i), uintPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        vecvec(intPtr(0, i), intPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(intPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecvec(intPtr(0, i), floatPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(intPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecvec(intPtr(0, i), doublePtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(intPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecvec(floatPtr(0, i), uintPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(floatPtr(0, i), uintPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecvec(floatPtr(0, i), intPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(floatPtr(0, i), intPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecvec(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(floatPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecvec(floatPtr(0, i), doublePtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(floatPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecvec(doublePtr(0, i), uintPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(doublePtr(0, i), uintPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecvec(doublePtr(0, i), intPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(doublePtr(0, i), intPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecvec(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(doublePtr(0, i), floatPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecvec(doublePtr(0, i), doublePtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(doublePtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            }
        }
    }
    else if (1 == H(0))                // vector * matrix
    {
        // first allocate backing memory
        backMem = allocBackMem(PREC, WIDTH, HEIGHT, SLOTS);

        // calculate and create fronts
        for (size_t i = 0; i < SLOTS; i++)
        {
            FrontMem* m = allocFrontMem(PREC, WIDTH, HEIGHT, backMem, i);

            frontMem.push_back(m);

            switch (prec0) {
            case (PrecType::UInt32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        uint32_t tmpAB[LEN];
                        vecmat(uintPtr(0, i), uintPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(uintPtr(0, i), uintPtr(1, i),
                               m->uintPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        vecmat(uintPtr(0, i), intPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(uintPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecmat(uintPtr(0, i), floatPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(uintPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecmat(uintPtr(0, i), doublePtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(uintPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        vecmat(intPtr(0, i), uintPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(intPtr(0, i), uintPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        vecmat(intPtr(0, i), intPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(intPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecmat(intPtr(0, i), floatPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(intPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecmat(intPtr(0, i), doublePtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(intPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecmat(floatPtr(0, i), uintPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(floatPtr(0, i), uintPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecmat(floatPtr(0, i), intPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(floatPtr(0, i), intPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        vecmat(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(floatPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecmat(floatPtr(0, i), doublePtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(floatPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecmat(doublePtr(0, i), uintPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(doublePtr(0, i), uintPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecmat(doublePtr(0, i), intPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(doublePtr(0, i), intPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecmat(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(doublePtr(0, i), floatPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        vecmat(doublePtr(0, i), doublePtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(doublePtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            }
        }
    }
    else if (1 == H(1))                // matrix * vector
    {
        // first allocate backing memory
        backMem = allocBackMem(PREC, WIDTH, HEIGHT, SLOTS);

        // calculate and create fronts
        for (size_t i = 0; i < SLOTS; i++)
        {
            FrontMem* m = allocFrontMem(PREC, WIDTH, HEIGHT, backMem, i);

            frontMem.push_back(m);

            switch (prec0) {
            case (PrecType::UInt32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        uint32_t tmpAB[LEN];
                        matvec(uintPtr(0, i), uintPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(uintPtr(0, i), uintPtr(1, i),
                               m->uintPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        matvec(uintPtr(0, i), intPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(uintPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matvec(uintPtr(0, i), floatPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(uintPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matvec(uintPtr(0, i), doublePtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(uintPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        matvec(intPtr(0, i), uintPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(intPtr(0, i), uintPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        matvec(intPtr(0, i), intPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(intPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matvec(intPtr(0, i), floatPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(intPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matvec(intPtr(0, i), doublePtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(intPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matvec(floatPtr(0, i), uintPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(floatPtr(0, i), uintPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matvec(floatPtr(0, i), intPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(floatPtr(0, i), intPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matvec(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(floatPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matvec(floatPtr(0, i), doublePtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(floatPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matvec(doublePtr(0, i), uintPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(doublePtr(0, i), uintPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matvec(doublePtr(0, i), intPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(doublePtr(0, i), intPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matvec(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(doublePtr(0, i), floatPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matvec(doublePtr(0, i), doublePtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(doublePtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            }
        }
    }
    else                               // matrix * matrix
    {
        // first allocate backing memory
        backMem = allocBackMem(PREC, WIDTH, HEIGHT, SLOTS);

        // calculate and create fronts
        for (size_t i = 0; i < SLOTS; i++)
        {
            FrontMem* m = allocFrontMem(PREC, WIDTH, HEIGHT, backMem, i);

            frontMem.push_back(m);

            switch (prec0) {
            case (PrecType::UInt32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        uint32_t tmpAB[LEN];
                        matmat(uintPtr(0, i), uintPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(uintPtr(0, i), uintPtr(1, i),
                               m->uintPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        matmat(uintPtr(0, i), intPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(uintPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matmat(uintPtr(0, i), floatPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(uintPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matmat(uintPtr(0, i), doublePtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(uintPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        matmat(intPtr(0, i), uintPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(intPtr(0, i), uintPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[LEN];
                        matmat(intPtr(0, i), intPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(intPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matmat(intPtr(0, i), floatPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(intPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matmat(intPtr(0, i), doublePtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(intPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matmat(floatPtr(0, i), uintPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(floatPtr(0, i), uintPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matmat(floatPtr(0, i), intPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(floatPtr(0, i), intPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[LEN];
                        matmat(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(floatPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matmat(floatPtr(0, i), doublePtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(floatPtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matmat(doublePtr(0, i), uintPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(doublePtr(0, i), uintPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matmat(doublePtr(0, i), intPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(doublePtr(0, i), intPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matmat(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(doublePtr(0, i), floatPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[LEN];
                        matmat(doublePtr(0, i), doublePtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(doublePtr(0, i), doublePtr(1, i),
                               m->doublePtr());
                    }
                    break;
                }
                break;
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpMatmulBase::InterpMatmulBase(const size_t inCount,
                                   const size_t outCount)
    : BaseInterp(inCount, outCount) { }

}; // namespace chai_internal
