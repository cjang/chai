// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpGather.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// gather1_floor, gather2_floor

void InterpGather::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);
    swizzle(1);
    if (2 == _N) swizzle(2);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(W(0), H(0), isDouble(0));

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(W(0), H(0), isDouble(0), backMem, i);

        frontMem.push_back(m);

        if (1 == _N)
        {
            if (isDouble(0)) {
                if (isDouble(1)) {
                    gather1_floor(doublePtr(0, i), doublePtr(1, i),
                                  m->doublePtr());
                } else {
                    gather1_floor(doublePtr(0, i), floatPtr(1, i),
                                  m->doublePtr());
                }
            } else {
                if (isDouble(1)) {
                    gather1_floor(floatPtr(0, i), doublePtr(1, i),
                                  m->floatPtr());
                } else {
                    gather1_floor(floatPtr(0, i), floatPtr(1, i),
                                  m->floatPtr());
                }
            }
        }

        if (2 == _N)
        {
            if (isDouble(0)) {
                if (isDouble(1)) {
                    if (isDouble(2)) {
                        gather2_floor(doublePtr(0, i),
                                      doublePtr(1, i),
                                      doublePtr(2, i),
                                      m->doublePtr());
                    } else {
                        gather2_floor(doublePtr(0, i),
                                      doublePtr(1, i),
                                      floatPtr(2, i),
                                      m->doublePtr());
                    }
                } else {
                    if (isDouble(2)) {
                        gather2_floor(doublePtr(0, i),
                                      floatPtr(1, i),
                                      doublePtr(2, i),
                                      m->doublePtr());
                    } else {
                        gather2_floor(doublePtr(0, i),
                                      floatPtr(1, i),
                                      floatPtr(2, i),
                                      m->doublePtr());
                    }
                }
            } else {
                if (isDouble(1)) {
                    if (isDouble(2)) {
                        gather2_floor(floatPtr(0, i),
                                      doublePtr(1, i),
                                      doublePtr(2, i),
                                      m->floatPtr());
                    } else {
                        gather2_floor(floatPtr(0, i),
                                      doublePtr(1, i),
                                      floatPtr(2, i),
                                      m->floatPtr());
                    }
                } else {
                    if (isDouble(2)) {
                        gather2_floor(floatPtr(0, i),
                                      floatPtr(1, i),
                                      doublePtr(2, i),
                                      m->floatPtr());
                    } else {
                        gather2_floor(floatPtr(0, i),
                                      floatPtr(1, i),
                                      floatPtr(2, i),
                                      m->floatPtr());
                    }
                }
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpGather::InterpGather(const size_t N)
    : BaseInterp(0, N + 1),
      _N(N),
      _fun() { }

}; // namespace chai_internal
