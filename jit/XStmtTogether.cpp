// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "XStmtCompound.hpp"
#include "XStmtCreateData.hpp"
#include "XStmtGather.hpp"
#include "XStmtIdSpace.hpp"
#include "XStmtIndex.hpp"
#include "XStmtLiteral.hpp"
#include "XStmtMaker.hpp"
#include "XStmtMatmul.hpp"
#include "XStmtReadData.hpp"
#include "XStmtReduce.hpp"
#include "XStmtRepeat.hpp"
#include "XStmtRNGrand.hpp"
#include "XStmtRNGseed.hpp"
#include "XStmtSendData.hpp"
#include "XStmtSingle.hpp"
#include "XStmtTogether.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// index space compatibility

bool XStmtTogether::isMatmul(void)
{
    if (_prevIsMatmul)
    {
        _prevIsMatmul = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool XStmtTogether::isEmpty(void) const
{
    return _togetherList.empty();
}

bool XStmtTogether::isPragmas(void) const
{
    // check for non-empty list with no dimensions yet
    return 0 == _indexW && 0 == _indexH && !_togetherList.empty();
}

bool XStmtTogether::isCode(void) const
{
    // check for dimensions which indicates there is code
    return 0 != _indexW || 0 != _indexH;
}

void XStmtTogether::boundIndex(const size_t w, const size_t h)
{
    if (w > _indexW) _indexW = w;
    if (h > _indexH) _indexH = h;
}

void XStmtTogether::boundIndex(const XStmtTogether& t)
{
    boundIndex(t.indexW(), t.indexH());
}

void XStmtTogether::boundIndex(const XStmt& s)
{
    boundIndex(s.lhsVariable()->W(), s.lhsVariable()->H());
}

void XStmtTogether::setIndex(const XStmtReadData& s)
{
    _indexW = s.lhsVariable()->W();
    _indexH = s.lhsVariable()->H();
}

void XStmtTogether::pushList(XStmt& s)
{
    _togetherList.push_back(&s);

    // some XStmt types have a NULL left hand side (e.g. compound)
    if (s.lhsVariable())
    {
        _lhsVariable.insert(s.lhsVariable());
        _allVariable.insert(s.lhsVariable());
    }

    _rhsVariable.insert(s.rhsVariable().begin(), s.rhsVariable().end());
    _allVariable.insert(s.rhsVariable().begin(), s.rhsVariable().end());
}

XStmtTogether::XStmtTogether(void)
    : _togetherList(),
      _indexW(0),
      _indexH(0),
      _prevIsMatmul(false) { }

void XStmtTogether::clear(void)
{
    _togetherList.clear();
    _allVariable.clear();
    _lhsVariable.clear();
    _rhsVariable.clear();
    _indexW = 0;
    _indexH = 0;
    _prevIsMatmul = false;
}

const vector< XStmt* >& XStmtTogether::stuffInside(void) const
{
    return _togetherList;
}

const set< AstVariable* >& XStmtTogether::allVariable(void) const
{
    return _allVariable;
}

const set< AstVariable* >& XStmtTogether::lhsVariable(void) const
{
    return _lhsVariable;
}

const set< AstVariable* >& XStmtTogether::rhsVariable(void) const
{
    return _rhsVariable;
}

size_t XStmtTogether::indexW(void) const
{
    return _indexW;
}

size_t XStmtTogether::indexH(void) const
{
    return _indexH;
}

bool XStmtTogether::isPush(XStmt* s)
{
    const size_t beforeLength = _togetherList.size();
    s->accept(*this);
    return beforeLength != _togetherList.size();
}

////////////////////////////////////////
// VisitXStmt

void XStmtTogether::visit(XStmtCompound& s)
{
    if (isMatmul()) return;
    if (isPragmas()) return;

    // recurse into compound to find dimensions
    XStmtTogether tmpTogether;
    for (vector< XStmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        (*it)->accept(tmpTogether);
    }

    boundIndex(tmpTogether);
    pushList(s);
}

void XStmtTogether::visit(XStmtCreateData& s)
{
    if (isMatmul()) return;
    if (! isCode()) pushList(s);
}

void XStmtTogether::visit(XStmtGather& s)
{
    if (isMatmul()) return;
    if (isPragmas()) return;
    boundIndex(s);
    pushList(s);
}

void XStmtTogether::visit(XStmtIdSpace& s)
{
    // should never happen
}

void XStmtTogether::visit(XStmtIndex& s)
{
    if (isMatmul()) return;
    if (isPragmas()) return;
    boundIndex(s);
    pushList(s);
}

void XStmtTogether::visit(XStmtLiteral& s)
{
    if (isMatmul()) return;
    if (isPragmas()) return;
    boundIndex(s);
    pushList(s);
}

void XStmtTogether::visit(XStmtMatmul& s)
{
    if (isMatmul()) return;
    if (isEmpty())
    {
        boundIndex(s);
        pushList(s);
        _prevIsMatmul = true;
    }
}

void XStmtTogether::visit(XStmtReadData& s)
{
    if (isMatmul()) return;
    if (isEmpty())
    {
        pushList(s);
    }
    else
    {
        // set the index space dimensions of code preceeding the readout
        setIndex(s);
    }
}

void XStmtTogether::visit(XStmtReduce& s)
{
    if (isMatmul()) return;
    if (isPragmas()) return;
    boundIndex(s);
    pushList(s);
}

void XStmtTogether::visit(XStmtRepeat& s)
{
    if (isMatmul()) return;
    if (isPragmas()) return;

    // recurse into compound to find dimensions
    XStmtTogether tmpTogether;
    for (vector< XStmt* >::const_iterator
         it = s.stuffInside()->stuffInside().begin();
         it != s.stuffInside()->stuffInside().end();
         it++)
    {
        (*it)->accept(tmpTogether);
    }

    boundIndex(tmpTogether);
    pushList(s);
}

void XStmtTogether::visit(XStmtRNGrand& s)
{
    if (isMatmul()) return;
    if (isPragmas()) return;
    boundIndex(s);
    pushList(s);
}

void XStmtTogether::visit(XStmtRNGseed& s)
{
    if (isMatmul()) return;
    if (! isCode()) pushList(s);
}

void XStmtTogether::visit(XStmtSendData& s)
{
    if (isMatmul()) return;
    if (! isCode()) pushList(s);
}

void XStmtTogether::visit(XStmtSingle& s)
{
    if (isMatmul()) return;
    if (isPragmas()) return;
    boundIndex(s);
    pushList(s);
}

}; // namespace chai_internal
