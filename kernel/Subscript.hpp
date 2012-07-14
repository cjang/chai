// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#ifndef _CHAI_SUBSCRIPT_HPP_
#define _CHAI_SUBSCRIPT_HPP_

#include <cstddef>
#include <ostream>
#include <string>

namespace chai_internal {

////////////////////////////////////////
// array subscript for kernel variables

class Subscript
{
    // data blocking between index space and outer loop
    enum TileBlocking
    {
        NO_TILE_BLOCKING,        // single threaded case
        OUTER_LOOP_OVER_TILES,   // index space within array stream
        GLOBAL_INDEX_OVER_TILES  // outer loop within array stream
    };

    TileBlocking _blocking;

    bool         _blockingEnabled;

    // index variable for outer loop over tiles
    size_t       _tileVar;

    // variable dimensions
    size_t       _varWidth;
    size_t       _varHeight;
    size_t       _varVecLength;

    // special modes
    bool         _transposed;
    bool         _outerProductLeft;
    bool         _outerProductRight;
    std::string  _gatherWidthIndex;
    std::string  _gatherHeightIndex;
    size_t       _eligibleGatherN;
    bool         _eligibleGatherXHasIndex;
    bool         _eligibleGatherYHasIndex;
    size_t       _eligibleGatherXVecOffset;
    size_t       _eligibleGatherYVecOffset;

    // mixed vector lengths in statement
    size_t       _mixedVecLength;
    size_t       _mixedComponentIdx;

    bool         _readScalar; // special case for read_scalar() variables

    bool         _subscriptBrackets;

protected:
    size_t varWidth(void) const;
    size_t varHeight(void) const;
    size_t varVecLength(void) const;

    bool isTransposed(void) const;
    bool isOuterProductLeft(void) const;
    bool isOuterProductRight(void) const;
    bool isGatherIndex(void) const;

    std::string getGatherWidthIndex(void) const;
    std::string getGatherHeightIndex(void) const;

    bool isEligibleGather(void) const;
    size_t eligibleGatherN(void) const;
    bool eligibleGatherXHasIndex(void) const;
    bool eligibleGatherYHasIndex(void) const;
    size_t eligibleGatherXVecOffset(void) const;
    size_t eligibleGatherYVecOffset(void) const;

    size_t getMixedVecLength(void) const;
    size_t getMixedComponentIdx(void) const;
    size_t mixedIndexSub(std::ostream& os) const;

    bool getSubscriptBrackets(void) const;

public:
    Subscript(void);
    virtual ~Subscript(void);

    bool isOuterLoopTiles(void) const;
    bool isGlobalIndexTiles(void) const;
    size_t getOuterLoopTileVar(void) const;

    void setOuterLoopTiles(const size_t loopVariable);
    void setGlobalIndexTiles(void);

    void disableBlocking(void);
    void enableBlocking(void);

    void setTranspose(void);
    void unsetTranspose(void);

    void setOuterProductLeft(void);
    void setOuterProductRight(void);
    void unsetOuterProduct(void);

    void setEligibleGather(const size_t N,
                           const bool xHasIndex,
                           const bool yHasIndex,
                           const size_t xVecOffset,
                           const size_t yVecOffset);
    void unsetEligibleGather(void);

    void setGatherWidthIndex(const std::string&);
    void setGatherHeightIndex(const std::string&);
    void unsetGatherIndex(void);

    void setMixedVecLength(const size_t vecLen);
    void setMixedComponentIdx(const size_t idx);
    void unsetMixedVecLength(void);

    void setVariable(const size_t width,
                     const size_t height,
                     const size_t vecLen);

    void setReadScalar(const bool v = true);

    bool isMixedVecLength(void) const;
    bool isMixedZero(void) const;
    void mixedComponentSub(std::ostream& os) const;

    void enableSubscriptBrackets(void);
    void disableSubscriptBrackets(void);

    virtual size_t boundingWidth(void) const = 0;
    virtual size_t boundingHeight(void) const = 0;
    virtual void arraySub(std::ostream& os) const = 0;
    virtual void widthIdx(std::ostream& os) const = 0;
    virtual void heightIdx(std::ostream& os) const = 0;
};

////////////////////////////////////////
// fixed subscript

class FixedSubscript : public Subscript
{
    const size_t _offset;

public:
    FixedSubscript(const size_t offset);

    size_t boundingWidth(void) const;
    size_t boundingHeight(void) const;
    void arraySub(std::ostream& os) const;
    void widthIdx(std::ostream& os) const;
    void heightIdx(std::ostream& os) const;
};

////////////////////////////////////////
// for-loop subscript

class ForLoopSubscript : public Subscript
{
    const size_t _widthLimit;
    const size_t _heightLimit;

    size_t       _widthVar;
    size_t       _heightVar;

public:
    ForLoopSubscript(const size_t widthLimit,
                     const size_t heightLimit);

    void setLoopVars(const size_t widthVar,
                     const size_t heightVar);

    size_t boundingWidth(void) const;
    size_t boundingHeight(void) const;
    void arraySub(std::ostream& os) const;
    void widthIdx(std::ostream& os) const;
    void heightIdx(std::ostream& os) const;
};

////////////////////////////////////////
// stream subscript

class StreamSubscript : public Subscript
{
    const size_t _globalWidth;
    const size_t _globalHeight;

public:
    StreamSubscript(const size_t globalWidth,
                    const size_t globalHeight);

    size_t boundingWidth(void) const;
    size_t boundingHeight(void) const;
    void arraySub(std::ostream& os) const;
    void widthIdx(std::ostream& os) const;
    void heightIdx(std::ostream& os) const;
};

}; // namespace chai_internal

#endif
