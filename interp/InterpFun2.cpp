// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpFun2.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// two argument array stream functions

void InterpFun2::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);
    swizzle(1);

    // first allocate backing memory
    const size_t maxW = max<size_t>(W(0), W(1));
    const size_t maxH = max<size_t>(H(0), H(1));
    BackMem* backMem = allocBackMem(maxW,
                                    maxH,
                                    isDouble(0) || isDouble(1));

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(maxW,
                                    maxH,
                                    isDouble(0) || isDouble(1),
                                    backMem,
                                    i);

        frontMem.push_back(m);

        if (isDouble(0))
        {
            if (isDouble(1))
            {
                for (size_t x = 0; x < maxW; x++)
                for (size_t y = 0; y < maxH; y++)
                    m->doublePtr()[ x + y * maxW ]
                        = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                doublePtr(1, i)[ idx(1, x, y) ] );
            }
            else
            {
                for (size_t x = 0; x < maxW; x++)
                for (size_t y = 0; y < maxH; y++)
                    m->doublePtr()[ x + y * maxW ]
                        = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                floatPtr(1, i)[ idx(1, x, y) ] );
            }
        }
        else
        {
            if (isDouble(1))
            {
                for (size_t x = 0; x < maxW; x++)
                for (size_t y = 0; y < maxH; y++)
                    m->doublePtr()[ x + y * maxW ]
                        = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                doublePtr(1, i)[ idx(1, x, y) ] );
            }
            else
            {
                for (size_t x = 0; x < maxW; x++)
                for (size_t y = 0; y < maxH; y++)
                    m->floatPtr()[ x + y * maxW ]
                        = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                floatPtr(1, i)[ idx(1, x, y) ] );
            }
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpFun2::InterpFun2(const SimpleFun2& f)
    : BaseInterp(0, 2),
      _fun(f) { }

}; // namespace chai_internal
