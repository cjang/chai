// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstVariable.hpp"
#include "BCStmtMaker.hpp"
#include "BCStmtCompound.hpp"
#include "BCStmtRepeat.hpp"
#include "BCStmtSingle.hpp"

using namespace std;

namespace chai_internal {

BCStmtMaker::BCStmtMaker(DispatchTrans& opDisp)
    : _opDisp(opDisp),
      _outStack()
{
    // VectorTrace context is set earlier by the Translator

    _opDisp.setContext(_outStack);
}

void BCStmtMaker::visit(BCStmtCompound& s)
{
    // visit each statement inside compound
    for (vector< BCStmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }
}

void BCStmtMaker::visit(BCStmtRepeat& s)
{
    // visit statement inside repeat object
    s.stuffInside()->accept(*this);
}

void BCStmtMaker::visit(BCStmtSingle& s)
{
    // clear the stream stack
    while (! _outStack.empty())
    {
        _outStack.pop();
    }

    // generate AST version of RHS
    s.rhsBytecode().ascend(_opDisp);

    // RHS could be empty if constructor or destructor
    if (_outStack.empty())
    {
        s.setAst(NULL);
    }
    else
    {
        s.setAst(
            new AstVariable(_outStack.top(),
                            s.lhsVariable(),
                            s.lhsVersion(),
                            s.lhsVariableIsLive(),
                            s.frontMem()) ); // statement memory
                                             // assigned to variable
    }
}

}; // namespace chai_internal
