// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpMatmulBase.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// matmul

#define VECVEC_GENERAL \
switch (prec2) \
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
switch (prec2) \
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
switch (prec2) \
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
switch (prec2) \
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

    const size_t prec0 = precision(0);
    const size_t prec1 = precision(1);
    const size_t prec2 = isGeneral ? precision(2) : -1;
    const size_t prec01 = prec0 < prec1 ? prec1 : prec0;

    BackMem* backMem;

    // array memory boxes
    vector< FrontMem* > frontMem;

    if ( (1 == H(0)) && (1 == H(1)) )  // vector * vector
    {
        // first allocate backing memory
        backMem = isGeneral
                      ? allocBackMem(W(2), H(2), prec2)
                      : allocBackMem(W(1), W(0), prec01);

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = isGeneral
                              ? allocFrontMem(W(2), H(2), prec2, backMem, i)
                              : allocFrontMem(W(1), W(0), prec01, backMem, i);

            frontMem.push_back(m);

            switch (prec0) {
            case (PrecType::UInt32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        uint32_t tmpAB[W(1) * W(0)];
                        vecvec(uintPtr(0, i), uintPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(uintPtr(0, i), uintPtr(1, i),
                               m->uintPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[W(1) * W(0)];
                        vecvec(uintPtr(0, i), intPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(uintPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1) * W(0)];
                        vecvec(uintPtr(0, i), floatPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(uintPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
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
                        int32_t tmpAB[W(1) * W(0)];
                        vecvec(intPtr(0, i), uintPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(intPtr(0, i), uintPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[W(1) * W(0)];
                        vecvec(intPtr(0, i), intPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(intPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1) * W(0)];
                        vecvec(intPtr(0, i), floatPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(intPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
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
                        float tmpAB[W(1) * W(0)];
                        vecvec(floatPtr(0, i), uintPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(floatPtr(0, i), uintPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        float tmpAB[W(1) * W(0)];
                        vecvec(floatPtr(0, i), intPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(floatPtr(0, i), intPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1) * W(0)];
                        vecvec(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(floatPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
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
                        double tmpAB[W(1) * W(0)];
                        vecvec(doublePtr(0, i), uintPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(doublePtr(0, i), uintPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
                        vecvec(doublePtr(0, i), intPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(doublePtr(0, i), intPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
                        vecvec(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        VECVEC_GENERAL
                    } else {
                        vecvec(doublePtr(0, i), floatPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
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
        backMem = isGeneral
                      ? allocBackMem(W(2), H(2), prec2)
                      : allocBackMem(W(1), 1, prec01);

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = isGeneral
                              ? allocFrontMem(W(2), H(2), prec2, backMem, i)
                              : allocFrontMem(W(1), 1, prec01, backMem, i);

            frontMem.push_back(m);

            switch (prec0) {
            case (PrecType::UInt32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        uint32_t tmpAB[W(1)];
                        vecmat(uintPtr(0, i), uintPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(uintPtr(0, i), uintPtr(1, i),
                               m->uintPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[W(1)];
                        vecmat(uintPtr(0, i), intPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(uintPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1)];
                        vecmat(uintPtr(0, i), floatPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(uintPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1)];
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
                        int32_t tmpAB[W(1)];
                        vecmat(intPtr(0, i), uintPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(intPtr(0, i), uintPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[W(1)];
                        vecmat(intPtr(0, i), intPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(intPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1)];
                        vecmat(intPtr(0, i), floatPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(intPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1)];
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
                        float tmpAB[W(1)];
                        vecmat(floatPtr(0, i), uintPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(floatPtr(0, i), uintPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        float tmpAB[W(1)];
                        vecmat(floatPtr(0, i), intPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(floatPtr(0, i), intPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1)];
                        vecmat(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(floatPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1)];
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
                        double tmpAB[W(1)];
                        vecmat(doublePtr(0, i), uintPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(doublePtr(0, i), uintPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        double tmpAB[W(1)];
                        vecmat(doublePtr(0, i), intPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(doublePtr(0, i), intPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        double tmpAB[W(1)];
                        vecmat(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        VECMAT_GENERAL
                    } else {
                        vecmat(doublePtr(0, i), floatPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1)];
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
        backMem = isGeneral
                      ? allocBackMem(W(2), H(2), prec2)
                      : allocBackMem(H(0), 1, prec01);

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = isGeneral
                              ? allocFrontMem(W(2), H(2), prec2, backMem, i)
                              : allocFrontMem(H(0), 1, prec01, backMem, i);

            frontMem.push_back(m);

            switch (prec0) {
            case (PrecType::UInt32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        uint32_t tmpAB[H(0)];
                        matvec(uintPtr(0, i), uintPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(uintPtr(0, i), uintPtr(1, i),
                               m->uintPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[H(0)];
                        matvec(uintPtr(0, i), intPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(uintPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[H(0)];
                        matvec(uintPtr(0, i), floatPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(uintPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[H(0)];
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
                        int32_t tmpAB[H(0)];
                        matvec(intPtr(0, i), uintPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(intPtr(0, i), uintPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[H(0)];
                        matvec(intPtr(0, i), intPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(intPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[H(0)];
                        matvec(intPtr(0, i), floatPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(intPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[H(0)];
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
                        float tmpAB[H(0)];
                        matvec(floatPtr(0, i), uintPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(floatPtr(0, i), uintPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        float tmpAB[H(0)];
                        matvec(floatPtr(0, i), intPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(floatPtr(0, i), intPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[H(0)];
                        matvec(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(floatPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[H(0)];
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
                        double tmpAB[H(0)];
                        matvec(doublePtr(0, i), uintPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(doublePtr(0, i), uintPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        double tmpAB[H(0)];
                        matvec(doublePtr(0, i), intPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(doublePtr(0, i), intPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        double tmpAB[H(0)];
                        matvec(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        MATVEC_GENERAL
                    } else {
                        matvec(doublePtr(0, i), floatPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[H(0)];
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
        backMem = isGeneral
                      ? allocBackMem(W(2), H(2), prec2)
                      : allocBackMem(W(1), H(0), prec01);

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = isGeneral
                              ? allocFrontMem(W(2), H(2), prec2, backMem, i)
                              : allocFrontMem(W(1), H(0), prec01, backMem, i);

            frontMem.push_back(m);

            switch (prec0) {
            case (PrecType::UInt32) :
                switch (prec1) {
                case (PrecType::UInt32) :
                    if (isGeneral) {
                        uint32_t tmpAB[W(1) * H(0)];
                        matmat(uintPtr(0, i), uintPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(uintPtr(0, i), uintPtr(1, i),
                               m->uintPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[W(1) * H(0)];
                        matmat(uintPtr(0, i), intPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(uintPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1) * H(0)];
                        matmat(uintPtr(0, i), floatPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(uintPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
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
                        int32_t tmpAB[W(1) * H(0)];
                        matmat(intPtr(0, i), uintPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(intPtr(0, i), uintPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        int32_t tmpAB[W(1) * H(0)];
                        matmat(intPtr(0, i), intPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(intPtr(0, i), intPtr(1, i),
                               m->intPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1) * H(0)];
                        matmat(intPtr(0, i), floatPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(intPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
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
                        float tmpAB[W(1) * H(0)];
                        matmat(floatPtr(0, i), uintPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(floatPtr(0, i), uintPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        float tmpAB[W(1) * H(0)];
                        matmat(floatPtr(0, i), intPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(floatPtr(0, i), intPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        float tmpAB[W(1) * H(0)];
                        matmat(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(floatPtr(0, i), floatPtr(1, i),
                               m->floatPtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
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
                        double tmpAB[W(1) * H(0)];
                        matmat(doublePtr(0, i), uintPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(doublePtr(0, i), uintPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Int32) :
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
                        matmat(doublePtr(0, i), intPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(doublePtr(0, i), intPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Float) :
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
                        matmat(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        MATMAT_GENERAL
                    } else {
                        matmat(doublePtr(0, i), floatPtr(1, i),
                               m->doublePtr());
                    }
                    break;
                case (PrecType::Double) :
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
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
