// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "InterpDotprod.hpp"
#include "PrecType.hpp"
#include "UtilFuns.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// dot_product

void InterpDotprod::sub_eval(stack< vector< FrontMem* > >& outStack)
{
    swizzle(0);
    swizzle(1);

    const size_t prec0  = prec(0);
    const size_t prec1  = prec(1);
    const size_t PREC   = max<size_t>(prec0, prec1);

    const size_t LEN    = max<size_t>(W(0), W(1)) * max<size_t>(H(0), H(1));
    const size_t SLOTS  = max<size_t>(slots(0), slots(1));

    // first allocate backing memory
    BackMem* backMem = allocBackMem(PREC, 1, 1, SLOTS);

    // array memory boxes
    vector< FrontMem* > frontMem;

    // calculate and create fronts
    for (size_t i = 0; i < SLOTS; i++)
    {
        FrontMem* m = allocFrontMem(PREC, 1, 1, backMem, i);

        frontMem.push_back(m);

        double accum = 0;

        switch (prec0)
        {
            case (PrecType::UInt32) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += uintPtr(0, i)[j] * uintPtr(1, i)[j];
                        break;

                    case (PrecType::Int32) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += uintPtr(0, i)[j] * intPtr(1, i)[j];
                        break;

                    case (PrecType::Float) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += uintPtr(0, i)[j] * floatPtr(1, i)[j];
                        break;

                    case (PrecType::Double) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += uintPtr(0, i)[j] * doublePtr(1, i)[j];
                        break;
                }
                break;

            case (PrecType::Int32) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += intPtr(0, i)[j] * uintPtr(1, i)[j];
                        break;

                    case (PrecType::Int32) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += intPtr(0, i)[j] * intPtr(1, i)[j];
                        break;

                    case (PrecType::Float) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += intPtr(0, i)[j] * floatPtr(1, i)[j];
                        break;

                    case (PrecType::Double) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += intPtr(0, i)[j] * doublePtr(1, i)[j];
                        break;
                }
                break;

            case (PrecType::Float) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += floatPtr(0, i)[j] * uintPtr(1, i)[j];
                        break;

                    case (PrecType::Int32) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += floatPtr(0, i)[j] * intPtr(1, i)[j];
                        break;

                    case (PrecType::Float) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += floatPtr(0, i)[j] * floatPtr(1, i)[j];
                        break;

                    case (PrecType::Double) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += floatPtr(0, i)[j] * doublePtr(1, i)[j];
                        break;
                }
                break;

            case (PrecType::Double) :
                switch (prec1)
                {
                    case (PrecType::UInt32) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += doublePtr(0, i)[j] * uintPtr(1, i)[j];
                        break;

                    case (PrecType::Int32) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += doublePtr(0, i)[j] * intPtr(1, i)[j];
                        break;

                    case (PrecType::Float) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += doublePtr(0, i)[j] * floatPtr(1, i)[j];
                        break;

                    case (PrecType::Double) :
                        for (size_t j = 0; j < LEN; j++)
                            accum += doublePtr(0, i)[j] * doublePtr(1, i)[j];
                        break;
                }
                break;
        }

        switch (PREC)
        {
            case (PrecType::UInt32) :
                m->uintPtr()[0] = accum;
                break;

            case (PrecType::Int32) :
                m->intPtr()[0] = accum;
                break;

            case (PrecType::Float) :
                m->floatPtr()[0] = accum;
                break;

            case (PrecType::Double) :
                m->doublePtr()[0] = accum;
                break;
        }
    }

    // push result on stack
    outStack.push(frontMem);
}

InterpDotprod::InterpDotprod(void)
    : BaseInterp(0, 2) { }

}; // namespace chai_internal
