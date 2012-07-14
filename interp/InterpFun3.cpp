// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpFun3.hpp"
#include "PrecType.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// three argument array stream functions

void InterpFun3::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);
    swizzle(1);
    swizzle(2);

    const size_t prec0  = prec(0);
    const size_t prec1  = prec(1);
    const size_t prec2  = prec(2);
    const size_t PREC   = max<size_t>(prec0, prec1, prec2);

    const size_t WIDTH  = max<size_t>(W(0), W(1), W(2));
    const size_t HEIGHT = max<size_t>(H(0), H(1), H(2));
    const size_t SLOTS  = max<size_t>(slots(0), slots(1), slots(2));

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
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->uintPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->intPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->intPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->intPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( uintPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            }
            break;
        case (PrecType::Int32) :
            switch (prec1)
            {
            case (PrecType::UInt32) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->intPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->intPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( intPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            }
            break;
        case (PrecType::Float) :
            switch (prec1)
            {
            case (PrecType::UInt32) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->floatPtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( floatPtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            }
            break;
        case (PrecType::Double) :
            switch (prec1)
            {
            case (PrecType::UInt32) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    uintPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    intPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    floatPtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec2)
                {
                case (PrecType::UInt32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    uintPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Int32) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    intPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Float) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    floatPtr(2, i)[ idx(2, x, y) ] );
                    break;
                case (PrecType::Double) :
                    for (size_t x = 0; x < WIDTH; x++)
                    for (size_t y = 0; y < HEIGHT; y++)
                        m->doublePtr()[ x + y * WIDTH ]
                            = _fun( doublePtr(0, i)[ idx(0, x, y) ],
                                    doublePtr(1, i)[ idx(1, x, y) ],
                                    doublePtr(2, i)[ idx(2, x, y) ] );
                    break;
                }
                break;
            }
            break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpFun3::InterpFun3(const SimpleFun3& f)
    : BaseInterp(0, 3),
      _fun(f) { }

}; // namespace chai_internal
