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

    const size_t prec0  = prec(0);
    const size_t prec1  = prec(1);
    const size_t PREC   = max<size_t>(prec0, prec1);

    const size_t WIDTH  = max<size_t>(W(0), W(1));
    const size_t HEIGHT = max<size_t>(H(0), H(1));

    const size_t slots0 = slots(0);
    const size_t slots1 = slots(1);
    const size_t SLOTS  = max<size_t>(slots(0), slots(1));

    // first allocate backing memory
    BackMem* backMem = allocBackMem(PREC, WIDTH, HEIGHT, SLOTS);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < SLOTS; i++)
    {
        FrontMem* m = allocFrontMem(PREC, WIDTH, HEIGHT, backMem, i);

        frontMem.push_back(m);

        switch (prec0)
        {
            case (PrecType::UInt32) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->uintPtr()[ x + y * WIDTH ]
                                = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                        uintPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Int32) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->intPtr()[ x + y * WIDTH ]
                                = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                        intPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Float) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->floatPtr()[ x + y * WIDTH ]
                                = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                        floatPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Double) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->doublePtr()[ x + y * WIDTH ]
                                = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                        doublePtr(1, i)[ idx(1, x, y) ] );
                        break;
                }
                break;

            case (PrecType::Int32) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->intPtr()[ x + y * WIDTH ]
                                = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                        uintPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Int32) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->intPtr()[ x + y * WIDTH ]
                                = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                        intPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Float) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->floatPtr()[ x + y * WIDTH ]
                                = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                        floatPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Double) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->doublePtr()[ x + y * WIDTH ]
                                = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                        doublePtr(1, i)[ idx(1, x, y) ] );
                        break;
                }
                break;

            case (PrecType::Float) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->floatPtr()[ x + y * WIDTH ]
                                = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                        uintPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Int32) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->floatPtr()[ x + y * WIDTH ]
                                = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                        intPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Float) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->floatPtr()[ x + y * WIDTH ]
                                = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                        floatPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Double) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->doublePtr()[ x + y * WIDTH ]
                                = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                        doublePtr(1, i)[ idx(1, x, y) ] );
                        break;
                }
                break;

            case (PrecType::Double) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->doublePtr()[ x + y * WIDTH ]
                                = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                        uintPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Int32) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->doublePtr()[ x + y * WIDTH ]
                                = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                        intPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Float) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->doublePtr()[ x + y * WIDTH ]
                                = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                        floatPtr(1, i)[ idx(1, x, y) ] );
                        break;

                    case (PrecType::Double) :
                        for (size_t x = 0; x < WIDTH; x++)
                        for (size_t y = 0; y < HEIGHT; y++)
                            m->doublePtr()[ x + y * WIDTH ]
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
