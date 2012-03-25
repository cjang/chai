// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpFun2.hpp"
#include "PrecType.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// two argument array stream functions

void InterpFun2::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);
    swizzle(1);

    const size_t prec0 = precision(0);
    const size_t prec1 = precision(1);
    const size_t precOut = prec0 < prec1 ? prec1 : prec0;

    // first allocate backing memory
    const size_t maxW = max<size_t>(W(0), W(1));
    const size_t maxH = max<size_t>(H(0), H(1));
    BackMem* backMem = allocBackMem(maxW, maxH, precOut);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < numTraces(); i++)
    {
        FrontMem* m = allocFrontMem(maxW, maxH, precOut, backMem, i);

        frontMem.push_back(m);

        switch (prec0)
        {
            case (PrecType::UInt32) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->uintPtr()[ x + y * maxW ]
                                = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                        uintPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Int32) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->intPtr()[ x + y * maxW ]
                                = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                        intPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Float) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->floatPtr()[ x + y * maxW ]
                                = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                        floatPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Double) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->doublePtr()[ x + y * maxW ]
                                = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                        doublePtr(1, i)[ idx(1, x, y) ] );
                        break;
                }
                break;

            case (PrecType::Int32) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->intPtr()[ x + y * maxW ]
                                = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                        uintPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Int32) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->intPtr()[ x + y * maxW ]
                                = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                        intPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Float) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->floatPtr()[ x + y * maxW ]
                                = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                        floatPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Double) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->doublePtr()[ x + y * maxW ]
                                = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                        doublePtr(1, i)[ idx(1, x, y) ] );
                        break;
                }
                break;

            case (PrecType::Float) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->floatPtr()[ x + y * maxW ]
                                = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                        uintPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Int32) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->floatPtr()[ x + y * maxW ]
                                = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                        intPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Float) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->floatPtr()[ x + y * maxW ]
                                = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                        floatPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Double) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->doublePtr()[ x + y * maxW ]
                                = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                        doublePtr(1, i)[ idx(1, x, y) ] );
                        break;
                }
                break;

            case (PrecType::Double) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->doublePtr()[ x + y * maxW ]
                                = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                        uintPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Int32) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->doublePtr()[ x + y * maxW ]
                                = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                        intPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Float) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->doublePtr()[ x + y * maxW ]
                                = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                        floatPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Double) :
                        for (size_t x = 0; x < maxW; x++)
                        for (size_t y = 0; y < maxH; y++)
                            m->doublePtr()[ x + y * maxW ]
                                = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                        doublePtr(1, i)[ idx(1, x, y) ] );
                        break;
                }
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpFun2::InterpFun2(const SimpleFun2& f)
    : BaseInterp(0, 2),
      _fun(f) { }

}; // namespace chai_internal
