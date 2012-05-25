// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>
#include <string>

#include "Gathering.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// gathering

////////////////////////////////////////
// variable description

Gathering::VariableDesc::VariableDesc(void)
    : _vectorLength(-1),
      _W(-1),
      _H(-1) { }

Gathering::VariableDesc::VariableDesc(const size_t vectorLength,
                                      const size_t width,
                                      const size_t height)
    : _vectorLength(vectorLength),
      _W(width),
      _H(height) { }

size_t Gathering::VariableDesc::vectorLength(void) const
{
    return _vectorLength;
}

size_t Gathering::VariableDesc::W(void) const
{
    return _W;
}

size_t Gathering::VariableDesc::H(void) const
{
    return _H;
}

size_t Gathering::vectorLength(const size_t variableNumber)
{
    return _counterToDesc[ variableNumber ].vectorLength();
}

////////////////////////////////////////
// application subscript

Gathering::AppSubscript::AppSubscript(void)
    : _N(-1),
      _xHasIndex(false),
      _yHasIndex(false),
      _xOffset(0),
      _yOffset(0) { }

Gathering::AppSubscript::AppSubscript(const size_t N,
                                      const bool xHasIndex,
                                      const bool yHasIndex,
                                      const size_t xOffset,
                                      const size_t yOffset)
    : _N(N),
      _xHasIndex(xHasIndex),
      _yHasIndex(yHasIndex),
      _xOffset(xOffset),
      _yOffset(yOffset) { }

bool Gathering::AppSubscript::operator< (const AppSubscript& other) const
{
    stringstream thisSS, otherSS;

    thisSS << _N << _xHasIndex << _yHasIndex
           << "_" << _xOffset << "_" << _yOffset;

    otherSS << other._N << other._xHasIndex << other._yHasIndex
            << "_" << other._xOffset << "_" << other._yOffset;

    return thisSS.str() < otherSS.str();
}

size_t Gathering::AppSubscript::N(void) const
{
    return _N;
}

bool Gathering::AppSubscript::xHasIndex(void) const
{
    return _xHasIndex;
}

bool Gathering::AppSubscript::yHasIndex(void) const
{
    return _yHasIndex;
}

size_t Gathering::AppSubscript::xOffset(void) const
{
    return _xOffset;
}

size_t Gathering::AppSubscript::yOffset(void) const
{
    return _yOffset;
}

////////////////////////////////////////
// array subscript

Gathering::ArraySubscript::ArraySubscript(void)
    : _variableNumber(-1),
      _N(-1),
      _xHasIndex(false),
      _yHasIndex(false),
      _xVecOffset(0),
      _yVecOffset(0) { }

Gathering::ArraySubscript::ArraySubscript(const size_t variableNumber,
                                          const size_t N,
                                          const bool xHasIndex,
                                          const size_t xVecOffset,
                                          const bool yHasIndex,
                                          const size_t yVecOffset)
    : _variableNumber(variableNumber),
      _N(N),
      _xHasIndex(xHasIndex),
      _yHasIndex(yHasIndex),
      _xVecOffset(xVecOffset),
      _yVecOffset(yVecOffset) { }

bool Gathering::ArraySubscript::operator< (const ArraySubscript& other) const
{
    stringstream thisSS, otherSS;

    thisSS << _N << _xHasIndex << _yHasIndex
           << "_" << _xVecOffset << "_" << _yVecOffset;

    otherSS << other._N << other._xHasIndex << other._yHasIndex
            << "_" << other._xVecOffset << "_" << other._yVecOffset;

    return thisSS.str() < otherSS.str();
}

size_t Gathering::ArraySubscript::variableNumber(void) const
{
    return _variableNumber;
}

size_t Gathering::ArraySubscript::N(void) const
{
    return _N;
}

bool Gathering::ArraySubscript::xHasIndex(void) const
{
    return _xHasIndex;
}

bool Gathering::ArraySubscript::yHasIndex(void) const
{
    return _yHasIndex;
}

size_t Gathering::ArraySubscript::xVecOffset(void) const
{
    return _xVecOffset;
}

size_t Gathering::ArraySubscript::yVecOffset(void) const
{
    return _yVecOffset;
}

////////////////////////////////////////
// public

Gathering::Gathering(void)
    : _variableCounter(0),
      _subscriptCounter(0) { }

Gathering::~Gathering(void) { }

size_t Gathering::gatherVariable(const uint32_t varNum,
                                 const size_t vectorLength,
                                 const size_t width,
                                 const size_t height)
{
    if ( _traceVarToCounter.count(varNum) )
    {
        return _traceVarToCounter[varNum];
    }

    _counterToDesc[ _variableCounter ] = VariableDesc( vectorLength,
                                                       width,
                                                       height );

    _traceVarToCounter[ varNum ] = _variableCounter;
    _traceCounterToVar[ _variableCounter ] = varNum;

    return _variableCounter++;
}

size_t Gathering::gatherVariable(const AstVariable* varPtr,
                                 const size_t vectorLength,
                                 const size_t width,
                                 const size_t height)
{
    if ( _splitVarToCounter.count(varPtr) )
    {
        return _splitVarToCounter[varPtr];
    }

    _counterToDesc[ _variableCounter ] = VariableDesc( vectorLength,
                                                       width,
                                                       height );

    _splitVarToCounter[ varPtr ] = _variableCounter;
    _splitCounterToVar[ _variableCounter ] = varPtr;

    return _variableCounter++;
}

size_t Gathering::gatherSubscript(const size_t variableNumber,
                                  const size_t N,
                                  const bool xHasIndex,
                                  const bool yHasIndex,
                                  const size_t xOffset,
                                  const size_t yOffset)
{
    const AppSubscript appSub(N, xHasIndex, yHasIndex, xOffset, yOffset);

    if ( _appSubscript[variableNumber].count(appSub) )
    {
        return _appSubscript[ variableNumber ][ appSub ];
    }

    const size_t effVecLen = vectorLength(variableNumber);

    const ArraySubscript arrSub(variableNumber,
                                N,
                                xHasIndex,
                                xOffset / effVecLen,
                                yHasIndex,
                                yOffset);

    if ( _subscriptToNumber.count(arrSub) )
    {
        return _appSubscript[ variableNumber ][ appSub ]
                   = _subscriptToNumber[ arrSub ];
    }

    _numberToSubscript[ _subscriptCounter ] = arrSub;
    _subscriptToNumber[ arrSub ] = _subscriptCounter;

    _appSubscript[ variableNumber ][ appSub ] = _subscriptCounter;

    return _subscriptCounter++;
}

size_t Gathering::numberSubscripts(void) const
{
    return _subscriptCounter;
}

uint32_t Gathering::traceVarFromSubscript(const size_t subNum)
{
    const size_t n = _numberToSubscript[subNum].variableNumber();
    return _traceCounterToVar.count(n)
               ? _traceCounterToVar[n]
               : -1;
}

const AstVariable* Gathering::splitVarFromSubscript(const size_t subNum)
{
    const size_t n = _numberToSubscript[subNum].variableNumber();
    return _splitCounterToVar.count(n)
               ? _splitCounterToVar[n]
               : NULL;
}

size_t Gathering::widthFromSubscript(const size_t subNum)
{
    const size_t n = _numberToSubscript[subNum].variableNumber();
    return _counterToDesc.count(n)
               ? _counterToDesc[n].W()
               : -1;
}

size_t Gathering::heightFromSubscript(const size_t subNum)
{
    const size_t n = _numberToSubscript[subNum].variableNumber();
    return _counterToDesc.count(n)
               ? _counterToDesc[n].H()
               : -1;
}

size_t Gathering::nFromSubscript(const size_t subNum)
{
    return _numberToSubscript[subNum].N();
}

bool Gathering::xHasIndexFromSubscript(const size_t subNum)
{
    return _numberToSubscript[subNum].xHasIndex();
}

bool Gathering::yHasIndexFromSubscript(const size_t subNum)
{
    return _numberToSubscript[subNum].yHasIndex();
}

size_t Gathering::xVecOffsetFromSubscript(const size_t subNum)
{
    return _numberToSubscript[subNum].xVecOffset();
}

size_t Gathering::yVecOffsetFromSubscript(const size_t subNum)
{
    return _numberToSubscript[subNum].yVecOffset();
}

}; // namespace chai_internal
