// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpMatmul.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// matmul

void InterpMatmul::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    BackMem* backMem;

    // array memory boxes
    vector< FrontMem* > frontMem;

    if ( (1 == H(0)) && (1 == H(1)) )  // vector * vector
    {
        // first allocate backing memory
        backMem = allocBackMem(W(1), W(0), isDouble(0) || isDouble(1));

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = allocFrontMem(W(1),
                                        W(0),
                                        isDouble(0) || isDouble(1),
                                        backMem,
                                        i);

            frontMem.push_back(m);

            if (isDouble(0)) {
                if (isDouble(1)) {
                    vecvec(doublePtr(0, i), doublePtr(1, i), m->doublePtr());
                } else {
                    vecvec(doublePtr(0, i), floatPtr(1, i), m->doublePtr());
                }
            } else {
                if (isDouble(1)) {
                    vecvec(floatPtr(0, i), doublePtr(1, i), m->doublePtr());
                } else {
                    vecvec(floatPtr(0, i), floatPtr(1, i), m->floatPtr());
                }
            }
        }
    }
    else if (1 == H(0))                // vector * matrix
    {
        // first allocate backing memory
        backMem = allocBackMem(W(1), 1, isDouble(0) || isDouble(1));

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = allocFrontMem(W(1),
                                        1,
                                        isDouble(0) || isDouble(1),
                                        backMem,
                                        i);

            frontMem.push_back(m);

            if (isDouble(0)) {
                if (isDouble(1)) {
                    vecmat(doublePtr(0, i), doublePtr(1, i), m->doublePtr());
                } else {
                    vecmat(doublePtr(0, i), floatPtr(1, i), m->doublePtr());
                }
            } else {
                if (isDouble(1)) {
                    vecmat(floatPtr(0, i), doublePtr(1, i), m->doublePtr());
                } else {
                    vecmat(floatPtr(0, i), floatPtr(1, i), m->floatPtr());
                }
            }
        }
    }
    else if (1 == H(1))                // matrix * vector
    {
        // first allocate backing memory
        backMem = allocBackMem(H(0), 1, isDouble(0) || isDouble(1));

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = allocFrontMem(H(0),
                                        1,
                                        isDouble(0) || isDouble(1),
                                        backMem,
                                        i);

            frontMem.push_back(m);

            if (isDouble(0)) {
                if (isDouble(1)) {
                    matvec(doublePtr(0, i), doublePtr(1, i), m->doublePtr());
                } else {
                    matvec(doublePtr(0, i), floatPtr(1, i), m->doublePtr());
                }
            } else {
                if (isDouble(1)) {
                    matvec(floatPtr(0, i), doublePtr(1, i), m->doublePtr());
                } else {
                    matvec(floatPtr(0, i), floatPtr(1, i), m->floatPtr());
                }
            }
        }
    }
    else                               // matrix * matrix
    {
        // first allocate backing memory
        backMem = allocBackMem(W(1), H(0), isDouble(0) || isDouble(1));

        // calculate and create fronts
        for (size_t i = 0; i < numTraces(); i++)
        {
            FrontMem* m = allocFrontMem(W(1),
                                        H(0),
                                        isDouble(0) || isDouble(1),
                                        backMem,
                                        i);

            frontMem.push_back(m);

            if (isDouble(0)) {
                if (isDouble(1)) {
                    matmat(doublePtr(0, i), doublePtr(1, i), m->doublePtr());
                } else {
                    matmat(doublePtr(0, i), floatPtr(1, i), m->doublePtr());
                }
            } else {
                if (isDouble(1)) {
                    matmat(floatPtr(0, i), doublePtr(1, i), m->doublePtr());
                } else {
                    matmat(floatPtr(0, i), floatPtr(1, i), m->floatPtr());
                }
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpMatmul::InterpMatmul(void)
    : BaseInterp(0, 2) { }

}; // namespace chai_internal
