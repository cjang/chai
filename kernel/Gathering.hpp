// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_GATHERING_HPP_
#define _CHAI_GATHERING_HPP_

#include <map>
#include <stdint.h>
#include <string>

namespace chai_internal {

class AstVariable;

////////////////////////////////////////
// gathering

class Gathering
{
    ////////////////////////////////////////
    // variable description

    class VariableDesc
    {
        size_t _vecLen;
        size_t _W;
        size_t _H;

    public:
        VariableDesc(void);
        VariableDesc(const size_t vecLen,
                     const size_t W,
                     const size_t H);

        size_t vecLength(void) const;
        size_t W(void) const;
        size_t H(void) const;
    };

    size_t _variableCounter;

    // description of each array variable
    std::map< size_t, VariableDesc > _counterToDesc;

    // assign unique number to each array variable
    std::map< uint32_t, size_t >           _traceVarToCounter;
    std::map< const AstVariable*, size_t > _splitVarToCounter;

    // reverse mapping from variable number
    std::map< size_t, uint32_t >           _traceCounterToVar;
    std::map< size_t, const AstVariable* > _splitCounterToVar;

    size_t vecLength(const size_t variableNumber);

    ////////////////////////////////////////
    // application subscript

    class AppSubscript
    {
        const size_t _N; // array is 1 or 2 dimensional
        const bool   _xHasIndex;
        const bool   _yHasIndex;
        const size_t _xOffset;
        const size_t _yOffset;

    public:
        AppSubscript(void);
        AppSubscript(const size_t N,
                     const bool xHasIndex,
                     const bool yHasIndex,
                     const size_t xOffset,
                     const size_t yOffset);

        bool operator< (const AppSubscript& other) const;

        size_t N(void) const;
        bool xHasIndex(void) const;
        bool yHasIndex(void) const;
        size_t xOffset(void) const;
        size_t yOffset(void) const;
    };

    // variableNumber -> appSubscript -> subscriptNumber
    // track variable subscripts (comes from application source code)
    std::map< size_t, std::map< AppSubscript, size_t > > _appSubscript;

    ////////////////////////////////////////
    // array subscript

    class ArraySubscript
    {
        size_t _variableNumber;
        size_t _N;
        bool   _xHasIndex;
        bool   _yHasIndex;
        size_t _xVecOffset;
        size_t _yVecOffset;

    public:
        ArraySubscript(void);
        ArraySubscript(const size_t variableNumber,
                       const size_t N,
                       const bool xHasIndex,
                       const size_t xVecOffset,
                       const bool yHasIndex,
                       const size_t yVecOffset);

        bool operator< (const ArraySubscript& other) const;

        size_t variableNumber(void) const;
        size_t N(void) const;
        bool xHasIndex(void) const;
        bool yHasIndex(void) const;
        size_t xVecOffset(void) const;
        size_t yVecOffset(void) const;
    };

    size_t _subscriptCounter;

    // subscriptNumber -> arraySubscript
    // track array subscripts
    std::map< size_t, ArraySubscript > _numberToSubscript;
    std::map< ArraySubscript, size_t > _subscriptToNumber;

public:
    Gathering(void);
    ~Gathering(void);

    // returns variable counter number
    size_t gatherVariable(const uint32_t varNum,
                          const size_t vecLen,
                          const size_t W,
                          const size_t H);

    // returns variable counter number
    size_t gatherVariable(const AstVariable* varPtr,
                          const size_t vecLen,
                          const size_t W,
                          const size_t H);

    // returns array subscript number
    size_t gatherSubscript(const size_t variableNumber,
                           const size_t N,
                           const bool xHasIndex,
                           const bool yHasIndex,
                           const size_t xOffset,
                           const size_t yOffset);

    size_t numberSubscripts(void) const;

    uint32_t traceVarFromSubscript(const size_t subNum);
    const AstVariable* splitVarFromSubscript(const size_t subNum);

    size_t widthFromSubscript(const size_t subNum);
    size_t heightFromSubscript(const size_t subNum);

    size_t nFromSubscript(const size_t subNum);
    bool xHasIndexFromSubscript(const size_t subNum);
    bool yHasIndexFromSubscript(const size_t subNum);
    size_t xVecOffsetFromSubscript(const size_t subNum);
    size_t yVecOffsetFromSubscript(const size_t subNum);
};

}; // namespace chai_internal

#endif
