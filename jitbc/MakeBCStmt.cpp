// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstVariable.hpp"
#include "BCStmtCompound.hpp"
#include "BCStmtRepeat.hpp"
#include "BCStmtSingle.hpp"
#include "MakeBCStmt.hpp"

using namespace std;

namespace chai_internal {

MakeBCStmt::MakeBCStmt(DispatchTrans& opDisp)
    : _opDisp(opDisp),
      _outStack(),
      _varScope()
{
    // VectorTrace context is set earlier by the Translator

    _opDisp.setContext(_outStack);
}

void MakeBCStmt::visit(BCStmtCompound& s)
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

void MakeBCStmt::visit(BCStmtRepeat& s)
{
    // visit statement inside repeat object
    s.stuffInside()->accept(*this);
}

void MakeBCStmt::visit(BCStmtSingle& s)
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

        if (0 == s.lhsVersion())
        {
            // constructor

            _varScope[ s.lhsVariable() ] = vector< BCStmtSingle* >();
        }
        else
        {
            // destructor

            // variable constructor and destructor may span kernels as lexical
            // scope is different from kernel scope - it is possible to have
            // a destructor without any corresponding statements
            if (! _varScope[ s.lhsVariable() ].empty())
            {
                _varScope[ s.lhsVariable() ].front()->setConstructorLHS();
                _varScope[ s.lhsVariable() ].back()->setDestructorLHS();
            }
        }
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

        _varScope[ s.lhsVariable() ].push_back( &s );
    }
}

}; // namespace chai_internal
