// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpMatmulBase.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// matmul

void InterpMatmulBase::doMatmul(stack< vector< FrontMem* > >& outStack)
{
    const bool isGeneral = 2 == _inCount &&  // alpha, beta
                           3 == _outCount;   // A, B, C

    swizzle(0);
    swizzle(1);
    if (isGeneral) swizzle(2);

    BackMem* backMem;

    // array memory boxes
    vector< FrontMem* > frontMem;

    if ( (1 == H(0)) && (1 == H(1)) )  // vector * vector
    {
        // first allocate backing memory
        backMem = isGeneral
                      ? allocBackMem(W(2), H(2), isDouble(2))
                      : allocBackMem(W(1), W(0), isDouble(0) || isDouble(1));

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = isGeneral
                              ? allocFrontMem(W(2),
                                              H(2),
                                              isDouble(2),
                                              backMem,
                                              i)
                              : allocFrontMem(W(1),
                                              W(0),
                                              isDouble(0) || isDouble(1),
                                              backMem,
                                              i);

            frontMem.push_back(m);

            if (isDouble(0)) {
                if (isDouble(1)) {
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
                        vecvec(doublePtr(0, i), doublePtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   W(0),
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   W(0),
                                   m->floatPtr());
                        }
                    } else {
                        vecvec(doublePtr(0, i),
                               doublePtr(1, i),
                               m->doublePtr());
                    }
                } else {
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
                        vecvec(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   W(0),
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   W(0),
                                   m->floatPtr());
                        }
                    } else {
                        vecvec(doublePtr(0, i),
                               floatPtr(1, i),
                               m->doublePtr());
                    }
                }
            } else {
                if (isDouble(1)) {
                    if (isGeneral) {
                        double tmpAB[W(1) * W(0)];
                        vecvec(floatPtr(0, i), doublePtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   W(0),
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   W(0),
                                   m->floatPtr());
                        }
                    } else {
                        vecvec(floatPtr(0, i),
                               doublePtr(1, i),
                               m->doublePtr());
                    }
                } else {
                    if (isGeneral) {
                        float tmpAB[W(1) * W(0)];
                        vecvec(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   W(0),
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   W(0),
                                   m->floatPtr());
                        }
                    } else {
                        vecvec(floatPtr(0, i),
                               floatPtr(1, i),
                               m->floatPtr());
                    }
                }
            }
        }
    }
    else if (1 == H(0))                // vector * matrix
    {
        // first allocate backing memory
        backMem = isGeneral
                      ? allocBackMem(W(2), H(2), isDouble(2))
                      : allocBackMem(W(1), 1, isDouble(0) || isDouble(1));

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = isGeneral
                              ? allocFrontMem(W(2),
                                              H(2),
                                              isDouble(2),
                                              backMem,
                                              i)
                              : allocFrontMem(W(1),
                                              1,
                                              isDouble(0) || isDouble(1),
                                              backMem,
                                              i);

            frontMem.push_back(m);

            if (isDouble(0)) {
                if (isDouble(1)) {
                    if (isGeneral) {
                        double tmpAB[W(1)];
                        vecmat(doublePtr(0, i), doublePtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   1,
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   1,
                                   m->floatPtr());
                        }
                    } else {
                        vecmat(doublePtr(0, i),
                               doublePtr(1, i),
                               m->doublePtr());
                    }
                } else {
                    if (isGeneral) {
                        double tmpAB[W(1)];
                        vecmat(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   1,
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   1,
                                   m->floatPtr());
                        }
                    } else {
                        vecmat(doublePtr(0, i),
                               floatPtr(1, i),
                               m->doublePtr());
                    }
                }
            } else {
                if (isDouble(1)) {
                    if (isGeneral) {
                        double tmpAB[W(1)];
                        vecmat(floatPtr(0, i), doublePtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   1,
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   1,
                                   m->floatPtr());
                        }
                    } else {
                        vecmat(floatPtr(0, i),
                               doublePtr(1, i),
                               m->doublePtr());
                    }
                } else {
                    if (isGeneral) {
                        float tmpAB[W(1)];
                        vecmat(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   1,
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   1,
                                   m->floatPtr());
                        }
                    } else {
                        vecmat(floatPtr(0, i),
                               floatPtr(1, i),
                               m->floatPtr());
                    }
                }
            }
        }
    }
    else if (1 == H(1))                // matrix * vector
    {
        // first allocate backing memory
        backMem = isGeneral
                      ? allocBackMem(W(2), H(2), isDouble(2))
                      : allocBackMem(H(0), 1, isDouble(0) || isDouble(1));

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = isGeneral
                              ? allocFrontMem(W(2),
                                              H(2),
                                              isDouble(2),
                                              backMem,
                                              i)
                              : allocFrontMem(H(0),
                                              1,
                                              isDouble(0) || isDouble(1),
                                              backMem,
                                              i);

            frontMem.push_back(m);

            if (isDouble(0)) {
                if (isDouble(1)) {
                    if (isGeneral) {
                        double tmpAB[H(0)];
                        matvec(doublePtr(0, i), doublePtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   H(0),
                                   1,
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   H(0),
                                   1,
                                   m->floatPtr());
                        }
                    } else {
                        matvec(doublePtr(0, i),
                               doublePtr(1, i),
                               m->doublePtr());
                    }
                } else {
                    if (isGeneral) {
                        double tmpAB[H(0)];
                        matvec(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   H(0),
                                   1,
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   H(0),
                                   1,
                                   m->floatPtr());
                        }
                    } else {
                        matvec(doublePtr(0, i),
                               floatPtr(1, i),
                               m->doublePtr());
                    }
                }
            } else {
                if (isDouble(1)) {
                    if (isGeneral) {
                        double tmpAB[H(0)];
                        matvec(floatPtr(0, i), doublePtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   H(0),
                                   1,
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   H(0),
                                   1,
                                   m->floatPtr());
                        }
                    } else {
                        matvec(floatPtr(0, i),
                               doublePtr(1, i),
                               m->doublePtr());
                    }
                } else {
                    if (isGeneral) {
                        float tmpAB[H(0)];
                        matvec(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   H(0),
                                   1,
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   H(0),
                                   1,
                                   m->floatPtr());
                        }
                    } else {
                        matvec(floatPtr(0, i),
                               floatPtr(1, i),
                               m->floatPtr());
                    }
                }
            }
        }
    }
    else                               // matrix * matrix
    {
        // first allocate backing memory
        backMem = isGeneral
                      ? allocBackMem(W(2), H(2), isDouble(2))
                      : allocBackMem(W(1), H(0), isDouble(0) || isDouble(1));

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = isGeneral
                              ? allocFrontMem(W(2),
                                              H(2),
                                              isDouble(2),
                                              backMem,
                                              i)
                              : allocFrontMem(W(1),
                                              H(0),
                                              isDouble(0) || isDouble(1),
                                              backMem,
                                              i);

            frontMem.push_back(m);

            if (isDouble(0)) {
                if (isDouble(1)) {
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
                        matmat(doublePtr(0, i), doublePtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   H(0),
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   H(0),
                                   m->floatPtr());
                        }
                    } else {
                        matmat(doublePtr(0, i),
                               doublePtr(1, i),
                               m->doublePtr());
                    }
                } else {
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
                        matmat(doublePtr(0, i), floatPtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   H(0),
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   H(0),
                                   m->floatPtr());
                        }
                    } else {
                        matmat(doublePtr(0, i),
                               floatPtr(1, i),
                               m->doublePtr());
                    }
                }
            } else {
                if (isDouble(1)) {
                    if (isGeneral) {
                        double tmpAB[W(1) * H(0)];
                        matmat(floatPtr(0, i), doublePtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   H(0),
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   H(0),
                                   m->floatPtr());
                        }
                    } else {
                        matmat(floatPtr(0, i),
                               doublePtr(1, i),
                               m->doublePtr());
                    }
                } else {
                    if (isGeneral) {
                        float tmpAB[W(1) * H(0)];
                        matmat(floatPtr(0, i), floatPtr(1, i), tmpAB);
                        if (isDouble(2)) {
                            addgen(tmpAB,
                                   doublePtr(2, i),
                                   W(1),
                                   H(0),
                                   m->doublePtr());
                        } else {
                            addgen(tmpAB,
                                   floatPtr(2, i),
                                   W(1),
                                   H(0),
                                   m->floatPtr());
                        }
                    } else {
                        matmat(floatPtr(0, i),
                               floatPtr(1, i),
                               m->floatPtr());
                    }
                }
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
