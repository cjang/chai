// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "Subscript.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// array subscript for kernel variables

size_t Subscript::varWidth(void) const
{
    return _varWidth;
}

size_t Subscript::varHeight(void) const
{
    return _varHeight;
}

size_t Subscript::varVecLength(void) const
{
    return _varVecLength;
}

bool Subscript::isTransposed(void) const
{
    return _transposed;
}

bool Subscript::isOuterProductLeft(void) const
{
    return _outerProductLeft;
}

bool Subscript::isOuterProductRight(void) const
{
    return _outerProductRight;
}

bool Subscript::isGatherIndex(void) const
{
    return ! _gatherWidthIndex.empty();
}

string Subscript::getGatherWidthIndex(void) const
{
    return _gatherWidthIndex;
}

string Subscript::getGatherHeightIndex(void) const
{
    return _gatherHeightIndex;
}

bool Subscript::isEligibleGather(void) const
{
    return -1 != _eligibleGatherN;
}

size_t Subscript::eligibleGatherN(void) const
{
    return _eligibleGatherN;
}

bool Subscript::eligibleGatherXHasIndex(void) const
{
    return _eligibleGatherXHasIndex;
}

bool Subscript::eligibleGatherYHasIndex(void) const
{
    return _eligibleGatherYHasIndex;
}

size_t Subscript::eligibleGatherXVecOffset(void) const
{
    return _eligibleGatherXVecOffset;
}

size_t Subscript::eligibleGatherYVecOffset(void) const
{
    return _eligibleGatherYVecOffset;
}

size_t Subscript::getMixedVecLength(void) const
{
    return _mixedVecLength;
}

size_t Subscript::getMixedComponentIdx(void) const
{
    return _mixedComponentIdx;
}

size_t Subscript::mixedIndexSub(ostream& os) const
{
    size_t largestWidth = boundingWidth();

    if (isMixedVecLength())
    {
        if (getMixedVecLength() < varVecLength())
        {
            // outer enclosing index range is for shorter vector elements
            os << " / " << (varVecLength() / getMixedVecLength());
            largestWidth /= (varVecLength() / getMixedVecLength());
        }
        else if (getMixedVecLength() > varVecLength())
        {
            // outer enclosing index range is for longer vector elements
            os << " * " << (getMixedVecLength() / varVecLength());
            largestWidth *= (getMixedVecLength() / varVecLength());
        }
    }

    return largestWidth;
}

bool Subscript::getSubscriptBrackets(void) const
{
    return _subscriptBrackets;
}

Subscript::Subscript(void)
    : _blocking(NO_TILE_BLOCKING),
      _blockingEnabled(true),
      _tileVar(-1),
      _varWidth(-1),
      _varHeight(-1),
      _varVecLength(-1),
      _transposed(false),
      _outerProductLeft(false),
      _outerProductRight(false),
      _gatherWidthIndex(),
      _gatherHeightIndex(),
      _eligibleGatherN(-1),
      _eligibleGatherXHasIndex(false),
      _eligibleGatherYHasIndex(false),
      _eligibleGatherXVecOffset(0),
      _eligibleGatherYVecOffset(0),
      _mixedVecLength(-1),
      _mixedComponentIdx(-1),
      _readScalar(false),
      _subscriptBrackets(true) { }

Subscript::~Subscript(void) { }

bool Subscript::isOuterLoopTiles(void) const
{
    return OUTER_LOOP_OVER_TILES == _blocking;
}

bool Subscript::isGlobalIndexTiles(void) const
{
    return GLOBAL_INDEX_OVER_TILES == _blocking;
}

size_t Subscript::getOuterLoopTileVar(void) const
{
    return _tileVar;
}

void Subscript::setOuterLoopTiles(const size_t loopVariable)
{
    _blocking = OUTER_LOOP_OVER_TILES;
    _tileVar = loopVariable;
}

void Subscript::setGlobalIndexTiles(void)
{
    _blocking = GLOBAL_INDEX_OVER_TILES;
}

void Subscript::disableBlocking(void)
{
    _blockingEnabled = false;
}

void Subscript::enableBlocking(void)
{
    _blockingEnabled = true;
}

void Subscript::setTranspose(void)
{
    _transposed = true;

    // exchange stream/variable width and height (transpose dimensions)
    size_t tmp = _varWidth;
    _varWidth = _varHeight;
    _varHeight = tmp;

    // If vector length is not 1, then pair/quad blocking for
    // index space IDs is necessary. A simple solution forces
    // the vector length to 1 anytime there is a transposed
    // matrix outside of matrix multiply. That leads to other
    // problems with images that have fixed vector lengths of
    // 2 (doubles) and 4 (floats). An even simpler approach
    // is to disallow this case entirely (transpose may appear
    // only as the A or B argument to matmul).
}

void Subscript::unsetTranspose(void)
{
    _transposed = false;

    // exchange stream/variable width and height (put it back to normal)
    size_t tmp = _varWidth;
    _varWidth = _varHeight;
    _varHeight = tmp;
}

void Subscript::setOuterProductLeft(void)
{
    _outerProductLeft = true;
    _outerProductRight = false;
}

void Subscript::setOuterProductRight(void)
{
    _outerProductLeft = false;
    _outerProductRight = true;
}

void Subscript::unsetOuterProduct(void)
{
    _outerProductLeft = false;
    _outerProductRight = false;
}

void Subscript::setGatherWidthIndex(const string& gatherWidthIndex)
{
    _gatherWidthIndex = gatherWidthIndex;
}

void Subscript::setGatherHeightIndex(const string& gatherHeightIndex)
{
    _gatherHeightIndex = gatherHeightIndex;
}

void Subscript::unsetGatherIndex(void)
{
    _gatherWidthIndex.clear();
    _gatherHeightIndex.clear();
}

void Subscript::setEligibleGather(const size_t N,
                                  const bool xHasIndex,
                                  const bool yHasIndex,
                                  const size_t xVecOffset,
                                  const size_t yVecOffset)
{
    _eligibleGatherN = N;
    _eligibleGatherXHasIndex = xHasIndex;
    _eligibleGatherYHasIndex = yHasIndex;
    _eligibleGatherXVecOffset = xVecOffset;
    _eligibleGatherYVecOffset = yVecOffset;
}

void Subscript::unsetEligibleGather(void)
{
    _eligibleGatherN = -1;
    _eligibleGatherXHasIndex = false;
    _eligibleGatherYHasIndex = false;
    _eligibleGatherXVecOffset = 0;
    _eligibleGatherYVecOffset = 0;
}

void Subscript::setMixedVecLength(const size_t vecLen)
{
    _mixedVecLength = vecLen;
}

void Subscript::setMixedComponentIdx(const size_t idx)
{
    _mixedComponentIdx = idx;
}

void Subscript::unsetMixedVecLength(void)
{
    _mixedVecLength = -1;
    _mixedComponentIdx = -1;
}

void Subscript::setVariable(const size_t width,
                            const size_t height,
                            const size_t vecLen)
{
    _varWidth = width;
    _varHeight = height;
    _varVecLength = vecLen;
}

void Subscript::setReadScalar(const bool v)
{
    _readScalar = v;
}

bool Subscript::isMixedVecLength(void) const
{
    return -1 != _mixedVecLength;
}

bool Subscript::isMixedZero(void) const
{
    if (isMixedVecLength() && -1 != getMixedComponentIdx())
    {
        return getMixedComponentIdx() >= varVecLength();
    }
    else
    {
        return false;
    }
}

void Subscript::mixedComponentSub(ostream& os) const
{
    if (isMixedVecLength() && -1 != getMixedComponentIdx())
    {
        if (getMixedVecLength() < varVecLength())
        {
            // outer enclosing index range is for shorter vector elements
            switch (getMixedVecLength())
            {
                case (1) :
                    os << ".s" << (getMixedComponentIdx() % varVecLength());
                    break;

                case (2) :
                    if (0 == (getMixedComponentIdx() % 2))
                        os << ".lo";
                    else
                        os << ".hi";
                    break;

                // mixed vector length can not be 4 as strictly smaller
            }
        }
        else if (getMixedVecLength() > varVecLength())
        {
            // outer enclosing index range is for longer vector elements
        }
    }
}

void Subscript::enableSubscriptBrackets(void)
{
    _subscriptBrackets = true;
}

void Subscript::disableSubscriptBrackets(void)
{
    _subscriptBrackets = false;
}

void Subscript::arraySub(ostream& os) const
{
    if (_blockingEnabled)
    {
        const size_t largestVecLength = 4;
        const size_t blockSizeInElems
            = _readScalar ? 1
                          : (_varWidth * _varHeight) / _varVecLength;

        switch (_blocking)
        {
            case (OUTER_LOOP_OVER_TILES) :
                os << " + i" << _tileVar
                   << " * " << blockSizeInElems;
                break;

            case (GLOBAL_INDEX_OVER_TILES) :
                os << " + get_global_id(0) * "
                   << blockSizeInElems;
        }
    }
}

void Subscript::heightIdx(ostream& os) const
{
    if (_blockingEnabled)
    {
        const size_t blockHeight = _readScalar ? 1
                                               : _varHeight;

        switch (_blocking)
        {
            case (OUTER_LOOP_OVER_TILES) :
                os << " + i" << _tileVar << " * " << blockHeight;
                break;

            case (GLOBAL_INDEX_OVER_TILES) :
                os << " + get_global_id(0) * " << blockHeight;
        }
    }
}

////////////////////////////////////////
// fixed subscript

FixedSubscript::FixedSubscript(const size_t offset)
    : Subscript(),
      _offset(offset) { }

size_t FixedSubscript::boundingWidth(void) const
{
    return 1;
}

size_t FixedSubscript::boundingHeight(void) const
{
    return 1;
}

void FixedSubscript::arraySub(ostream& os) const
{
    os << (getSubscriptBrackets() ? "[" : "") << _offset;

    Subscript::arraySub(os);

    os << (getSubscriptBrackets() ? "]" : "");

    mixedComponentSub(os);
}

void FixedSubscript::widthIdx(ostream& os) const
{
    os << _offset;
}

void FixedSubscript::heightIdx(ostream& os) const
{
    os << 0;

    Subscript::heightIdx(os);
}

////////////////////////////////////////
// for-loop subscript

ForLoopSubscript::ForLoopSubscript(const size_t widthLimit,
                                   const size_t heightLimit)
    : Subscript(),
      _widthLimit(widthLimit),
      _heightLimit(heightLimit),
      _widthVar(0),
      _heightVar(0) { }

void ForLoopSubscript::setLoopVars(const size_t widthVar,
                                   const size_t heightVar)
{
    _widthVar = widthVar;
    _heightVar = heightVar;
}

size_t ForLoopSubscript::boundingWidth(void) const
{
    return isTransposed()
               ? _heightLimit
               : _widthLimit;
}

size_t ForLoopSubscript::boundingHeight(void) const
{
    return isTransposed()
               ? _widthLimit
               : _heightLimit;
}

void ForLoopSubscript::arraySub(ostream& os) const
{
    os << (getSubscriptBrackets() ? "[" : "");

    if (isOuterProductLeft())
    {
        // vector length must be 1 for this to work correctly as
        // the outer product left vector is really transposed
        const_cast< ForLoopSubscript* >(this)->disableBlocking();
        heightIdx(os);
        const_cast< ForLoopSubscript* >(this)->enableBlocking();
    }
    else if (isOuterProductRight())
    {
        widthIdx(os);
    }
    else
    {
        const size_t y = isTransposed()
                             ? _widthVar
                             : _heightVar;

        // This could be a problem if the vector length is not 1
        // and a matrix is transposed.
        const size_t width = varWidth() / varVecLength();
        const size_t height = varHeight();

        widthIdx(os);

        if (-1 != y)
        {
            os << " + ";

            const_cast< ForLoopSubscript* >(this)->disableBlocking();
            heightIdx(os);
            const_cast< ForLoopSubscript* >(this)->enableBlocking();

            os << " * " << width;
        }
    }

    Subscript::arraySub(os);
    os << (getSubscriptBrackets() ? "]" : "");

    mixedComponentSub(os);
}

void ForLoopSubscript::widthIdx(ostream& os) const
{
    const size_t x = isTransposed()
                         ? _heightVar
                         : _widthVar;

    // This could be a problem if the vector length is not 1 and
    // a matrix is transposed.
    const size_t width = varWidth() / varVecLength();

    if (isEligibleGather())
    {
        const size_t gatherX = eligibleGatherXVecOffset();

        if (eligibleGatherXHasIndex())
        {
            os << "((i" << x << " + " << gatherX << ") % " << width << ")";
        }
        else
        {
            os << gatherX;
        }
    }
    else
    {
        stringstream ss;

        if (isGatherIndex())
            ss << "convert_int_rtn(" << getGatherWidthIndex() << ")";
        else
            ss << "i" << x;

        const size_t largestWidth = mixedIndexSub(ss);

        if (largestWidth <= width && ! isGatherIndex())
            os << "(" << ss.str() << ")";
        else
            os << "(" << ss.str() << " % " << width << ")";
    }
}

void ForLoopSubscript::heightIdx(ostream& os) const
{
    const size_t y = isTransposed()
                         ? _widthVar
                         : _heightVar;

    // This could be a problem if the vector length is not 1 and
    // a matrix is transposed.
    const size_t width = varWidth() / varVecLength();
    const size_t height = varHeight();

    if (isEligibleGather())
    {
        if (1 == eligibleGatherN())
        {
            os << 0;
        }
        else
        {
            const size_t gatherY = eligibleGatherYVecOffset();

            if (eligibleGatherYHasIndex())
            {
                os << "((i" << y << " + " << gatherY << ") % " << height << ")";
            }
            else
            {
                os << gatherY;
            }
        }
    }
    else
    {
        stringstream ss;

        if (isGatherIndex() && ! getGatherHeightIndex().empty())
            ss << "convert_int_rtn(" << getGatherHeightIndex() << ")";
        else
            ss << "i" << y;

        if (-1 != y)
        {
            if (isOuterProductLeft())
            {
                if (boundingWidth() <= width && ! isGatherIndex())
                    os << ss.str();
                else
                    os << "(" << ss.str() << " % " << width << ")";
            }
            else
            {
                if (boundingHeight() <= height && ! isGatherIndex())
                    os << ss.str();
                else
                    os << "(" << ss.str() << " % " << height << ")";
            }
        }
        else
        {
            os << 0;
        }
    }

    Subscript::heightIdx(os);
}

////////////////////////////////////////
// stream subscript

StreamSubscript::StreamSubscript(const size_t globalWidth,
                                 const size_t globalHeight)
    : Subscript(),
      _globalWidth(globalWidth),
      _globalHeight(globalHeight) { }

size_t StreamSubscript::boundingWidth(void) const
{
    return isTransposed()
               ? _globalHeight
               : _globalWidth;
}

size_t StreamSubscript::boundingHeight(void) const
{
    return isTransposed()
               ? _globalWidth
               : _globalHeight;
}

void StreamSubscript::arraySub(ostream& os) const
{
    os << (getSubscriptBrackets() ? "[" : "");

    if (isOuterProductLeft())
    {
        // vector length must be 1 for this to work correctly as
        // the outer product left vector is really transposed
        const_cast< StreamSubscript* >(this)->disableBlocking();
        heightIdx(os);
        const_cast< StreamSubscript* >(this)->enableBlocking();
    }
    else if (isOuterProductRight())
    {
        widthIdx(os);
    }
    else
    {
        // More problems if a matrix is transposed and vector length
        // is not 1... Just adding a transposed matrix to a normal one
        // is problematic.
        const size_t width = varWidth() / varVecLength();

        widthIdx(os);

        os << " + ";

        const_cast< StreamSubscript* >(this)->disableBlocking();
        heightIdx(os);
        const_cast< StreamSubscript* >(this)->enableBlocking();

        os << " * " << width;
    }

    Subscript::arraySub(os);
    os << (getSubscriptBrackets() ? "]" : "");

    mixedComponentSub(os);
}

void StreamSubscript::widthIdx(ostream& os) const
{
    const size_t x = isTransposed() ? 1 : 0;

    const size_t padW
        = (0 != (varWidth() % varVecLength()))
          ? varVecLength() * (1 + (varWidth() / varVecLength()))
          : varWidth();

    // More problems if a matrix is transposed and the vector length
    // is not 1... Just adding a transposed matrix to a normal one
    // is problematic.
    const size_t width = padW / varVecLength();

    if (isEligibleGather())
    {
        const size_t gatherX = eligibleGatherXVecOffset();

        if (eligibleGatherXHasIndex())
        {
            os << "((get_global_id(" << x << ") + " << gatherX << ") % "
               << width << ")";
        }
        else
        {
            os << gatherX;
        }
    }
    else
    {
        stringstream ss;

        if (isGatherIndex())
            ss << "convert_int_rtn(" << getGatherWidthIndex() << ")";
        else
            ss << "get_global_id(" << x << ")";

        const size_t largestWidth = mixedIndexSub(ss);

        if (largestWidth <= width && ! isGatherIndex())
            os << "(" << ss.str() << ")";
        else
            os << "((" << ss.str() << ") % " << width << ")";
    }
}

void StreamSubscript::heightIdx(ostream& os) const
{
    const size_t y = isTransposed() ? 0 : 1;

    // Problems if a matrix is transposed and the vector length
    // is not 1... Just adding a transposed matrix to a normal one
    // is problematic.
    const size_t width = varWidth() / varVecLength();
    const size_t height = varHeight();

    if (isEligibleGather())
    {
        if (1 == eligibleGatherN())
        {
            os << 0;
        }
        else
        {
            const size_t gatherY = eligibleGatherYVecOffset();

            if (eligibleGatherYHasIndex())
            {
                os << "((get_global_id(" << y << ") + " << gatherY
                   << ") % " << height << ")";
            }
            else
            {
                os << gatherY;
            }
        }
    }
    else
    {
        stringstream ss;

        if (isGatherIndex() && ! getGatherHeightIndex().empty())
            ss << "convert_int_rtn(" << getGatherHeightIndex() << ")";
        else
            ss << "get_global_id(" << y << ")";

        if (isOuterProductLeft())
        {
            if (boundingWidth() <= width && ! isGatherIndex())
                os << ss.str();
            else
                os << "(" << ss.str() << " % " << width << ")";
        }
        else
        {
            if (boundingHeight() <= height && ! isGatherIndex())
                os << ss.str();
            else
                os << "(" << ss.str() << " % " << height << ")";
        }
    }

    Subscript::heightIdx(os);
}

}; // namespace chai_internal
