// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpGather.hpp"
#include "PrecType.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// gather1_floor, gather2_floor

void InterpGather::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);
    swizzle(1);
    if (2 == _N) swizzle(2);

    const size_t prec0  = prec(0);
    const size_t prec1  = prec(1);
    const size_t prec2  = (2 == _N) ? prec(2) : -1;
    const size_t PREC   = prec0;

    const size_t WIDTH  = W(0);
    const size_t HEIGHT = H(0);
    const size_t SLOTS  = slots(0);

    // first allocate backing memory
    BackMem* backMem = allocBackMem(PREC, WIDTH, HEIGHT, SLOTS);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < SLOTS; i++)
    {
        FrontMem* m = allocFrontMem(PREC, WIDTH, HEIGHT, backMem, i);

        frontMem.push_back(m);

        if (1 == _N)
        {
            switch (PREC)
            {
            case (PrecType::UInt32) :
                switch (prec1)
                {
                case (PrecType::UInt32) :
                    gather1_floor(uintPtr(0, i), uintPtr(1, i),
                                  m->uintPtr());
                    break;
                case (PrecType::Int32) :
                    gather1_floor(uintPtr(0, i), intPtr(1, i),
                                  m->uintPtr());
                    break;
                case (PrecType::Float) :
                    gather1_floor(uintPtr(0, i), floatPtr(1, i),
                                  m->uintPtr());
                    break;
                case (PrecType::Double) :
                    gather1_floor(uintPtr(0, i), doublePtr(1, i),
                                  m->uintPtr());
                    break;
                }
                break;
            case (PrecType::Int32) :
                switch (prec1)
                {
                case (PrecType::UInt32) :
                    gather1_floor(intPtr(0, i), uintPtr(1, i),
                                  m->intPtr());
                    break;
                case (PrecType::Int32) :
                    gather1_floor(intPtr(0, i), intPtr(1, i),
                                  m->intPtr());
                    break;
                case (PrecType::Float) :
                    gather1_floor(intPtr(0, i), floatPtr(1, i),
                                  m->intPtr());
                    break;
                case (PrecType::Double) :
                    gather1_floor(intPtr(0, i), doublePtr(1, i),
                                  m->intPtr());
                    break;
                }
                break;
            case (PrecType::Float) :
                switch (prec1)
                {
                case (PrecType::UInt32) :
                    gather1_floor(floatPtr(0, i), uintPtr(1, i),
                                  m->floatPtr());
                    break;
                case (PrecType::Int32) :
                    gather1_floor(floatPtr(0, i), intPtr(1, i),
                                  m->floatPtr());
                    break;
                case (PrecType::Float) :
                    gather1_floor(floatPtr(0, i), floatPtr(1, i),
                                  m->floatPtr());
                    break;
                case (PrecType::Double) :
                    gather1_floor(floatPtr(0, i), doublePtr(1, i),
                                  m->floatPtr());
                    break;
                }
                break;
            case (PrecType::Double) :
                switch (prec1)
                {
                case (PrecType::UInt32) :
                    gather1_floor(doublePtr(0, i), uintPtr(1, i),
                                  m->doublePtr());
                    break;
                case (PrecType::Int32) :
                    gather1_floor(doublePtr(0, i), intPtr(1, i),
                                  m->doublePtr());
                    break;
                case (PrecType::Float) :
                    gather1_floor(doublePtr(0, i), floatPtr(1, i),
                                  m->doublePtr());
                    break;
                case (PrecType::Double) :
                    gather1_floor(doublePtr(0, i), doublePtr(1, i),
                                  m->doublePtr());
                    break;
                }
                break;
            }
        }

        if (2 == _N)
        {
            switch (PREC)
            {
            case (PrecType::UInt32) :
                switch (prec1)
                {
                case (PrecType::UInt32) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(uintPtr(0, i),
                                      uintPtr(1, i),
                                      uintPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(uintPtr(0, i),
                                      uintPtr(1, i),
                                      intPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(uintPtr(0, i),
                                      uintPtr(1, i),
                                      floatPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(uintPtr(0, i),
                                      uintPtr(1, i),
                                      doublePtr(2, i),
                                      m->uintPtr());
                        break;
                    }
                    break;
                case (PrecType::Int32) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(uintPtr(0, i),
                                      intPtr(1, i),
                                      uintPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(uintPtr(0, i),
                                      intPtr(1, i),
                                      intPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(uintPtr(0, i),
                                      intPtr(1, i),
                                      floatPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(uintPtr(0, i),
                                      intPtr(1, i),
                                      doublePtr(2, i),
                                      m->uintPtr());
                        break;
                    }
                    break;
                case (PrecType::Float) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(uintPtr(0, i),
                                      floatPtr(1, i),
                                      uintPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(uintPtr(0, i),
                                      floatPtr(1, i),
                                      intPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(uintPtr(0, i),
                                      floatPtr(1, i),
                                      floatPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(uintPtr(0, i),
                                      floatPtr(1, i),
                                      doublePtr(2, i),
                                      m->uintPtr());
                        break;
                    }
                    break;
                case (PrecType::Double) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(uintPtr(0, i),
                                      doublePtr(1, i),
                                      uintPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(uintPtr(0, i),
                                      doublePtr(1, i),
                                      intPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(uintPtr(0, i),
                                      doublePtr(1, i),
                                      floatPtr(2, i),
                                      m->uintPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(uintPtr(0, i),
                                      doublePtr(1, i),
                                      doublePtr(2, i),
                                      m->uintPtr());
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
                        gather2_floor(intPtr(0, i),
                                      uintPtr(1, i),
                                      uintPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(intPtr(0, i),
                                      uintPtr(1, i),
                                      intPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(intPtr(0, i),
                                      uintPtr(1, i),
                                      floatPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(intPtr(0, i),
                                      uintPtr(1, i),
                                      doublePtr(2, i),
                                      m->intPtr());
                        break;
                    }
                    break;
                case (PrecType::Int32) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(intPtr(0, i),
                                      intPtr(1, i),
                                      uintPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(intPtr(0, i),
                                      intPtr(1, i),
                                      intPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(intPtr(0, i),
                                      intPtr(1, i),
                                      floatPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(intPtr(0, i),
                                      intPtr(1, i),
                                      doublePtr(2, i),
                                      m->intPtr());
                        break;
                    }
                    break;
                case (PrecType::Float) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(intPtr(0, i),
                                      floatPtr(1, i),
                                      uintPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(intPtr(0, i),
                                      floatPtr(1, i),
                                      intPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(intPtr(0, i),
                                      floatPtr(1, i),
                                      floatPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(intPtr(0, i),
                                      floatPtr(1, i),
                                      doublePtr(2, i),
                                      m->intPtr());
                        break;
                    }
                    break;
                case (PrecType::Double) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(intPtr(0, i),
                                      doublePtr(1, i),
                                      uintPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(intPtr(0, i),
                                      doublePtr(1, i),
                                      intPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(intPtr(0, i),
                                      doublePtr(1, i),
                                      floatPtr(2, i),
                                      m->intPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(intPtr(0, i),
                                      doublePtr(1, i),
                                      doublePtr(2, i),
                                      m->intPtr());
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
                        gather2_floor(floatPtr(0, i),
                                      uintPtr(1, i),
                                      uintPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(floatPtr(0, i),
                                      uintPtr(1, i),
                                      intPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(floatPtr(0, i),
                                      uintPtr(1, i),
                                      floatPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(floatPtr(0, i),
                                      uintPtr(1, i),
                                      doublePtr(2, i),
                                      m->floatPtr());
                        break;
                    }
                    break;
                case (PrecType::Int32) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(floatPtr(0, i),
                                      intPtr(1, i),
                                      uintPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(floatPtr(0, i),
                                      intPtr(1, i),
                                      intPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(floatPtr(0, i),
                                      intPtr(1, i),
                                      floatPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(floatPtr(0, i),
                                      intPtr(1, i),
                                      doublePtr(2, i),
                                      m->floatPtr());
                        break;
                    }
                    break;
                case (PrecType::Float) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(floatPtr(0, i),
                                      floatPtr(1, i),
                                      uintPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(floatPtr(0, i),
                                      floatPtr(1, i),
                                      intPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(floatPtr(0, i),
                                      floatPtr(1, i),
                                      floatPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(floatPtr(0, i),
                                      floatPtr(1, i),
                                      doublePtr(2, i),
                                      m->floatPtr());
                        break;
                    }
                    break;
                case (PrecType::Double) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(floatPtr(0, i),
                                      doublePtr(1, i),
                                      uintPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(floatPtr(0, i),
                                      doublePtr(1, i),
                                      intPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(floatPtr(0, i),
                                      doublePtr(1, i),
                                      floatPtr(2, i),
                                      m->floatPtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(floatPtr(0, i),
                                      doublePtr(1, i),
                                      doublePtr(2, i),
                                      m->floatPtr());
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
                        gather2_floor(doublePtr(0, i),
                                      uintPtr(1, i),
                                      uintPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(doublePtr(0, i),
                                      uintPtr(1, i),
                                      intPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(doublePtr(0, i),
                                      uintPtr(1, i),
                                      floatPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(doublePtr(0, i),
                                      uintPtr(1, i),
                                      doublePtr(2, i),
                                      m->doublePtr());
                        break;
                    }
                    break;
                case (PrecType::Int32) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(doublePtr(0, i),
                                      intPtr(1, i),
                                      uintPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(doublePtr(0, i),
                                      intPtr(1, i),
                                      intPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(doublePtr(0, i),
                                      intPtr(1, i),
                                      floatPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(doublePtr(0, i),
                                      intPtr(1, i),
                                      doublePtr(2, i),
                                      m->doublePtr());
                        break;
                    }
                    break;
                case (PrecType::Float) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(doublePtr(0, i),
                                      floatPtr(1, i),
                                      uintPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(doublePtr(0, i),
                                      floatPtr(1, i),
                                      intPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(doublePtr(0, i),
                                      floatPtr(1, i),
                                      floatPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(doublePtr(0, i),
                                      floatPtr(1, i),
                                      doublePtr(2, i),
                                      m->doublePtr());
                        break;
                    }
                    break;
                case (PrecType::Double) :
                    switch (prec2)
                    {
                    case (PrecType::UInt32) :
                        gather2_floor(doublePtr(0, i),
                                      doublePtr(1, i),
                                      uintPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Int32) :
                        gather2_floor(doublePtr(0, i),
                                      doublePtr(1, i),
                                      intPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Float) :
                        gather2_floor(doublePtr(0, i),
                                      doublePtr(1, i),
                                      floatPtr(2, i),
                                      m->doublePtr());
                        break;
                    case (PrecType::Double) :
                        gather2_floor(doublePtr(0, i),
                                      doublePtr(1, i),
                                      doublePtr(2, i),
                                      m->doublePtr());
                        break;
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

InterpGather::InterpGather(const size_t N)
    : BaseInterp(0, N + 1),
      _N(N),
      _fun() { }

}; // namespace chai_internal
