// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>
#include <sstream>
#include <vector>

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstExtension.hpp"
#include "AstFun1.hpp"
#include "AstFun2.hpp"
#include "AstFun3.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstLitdata.hpp"
#include "AstMakedata.hpp"
#include "AstMatmulMM.hpp"
#include "AstMatmulMV.hpp"
#include "AstMatmulVM.hpp"
#include "AstMatmulVV.hpp"
#include "AstOpenCL.hpp"
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstTranspose.hpp"
#include "AstVariable.hpp"
#include "Enqueue.hpp"
#include "PrecType.hpp"
#include "SourceKernel.hpp"
#include "Variable.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// VisitAst

void Enqueue::visit(AstAccum& v)
{
    // reduction statement requires multiple statements
    // AST object is used for the statement inside the accumulation loop
    // note nested reductions are impossible due to statement splits
    descendAst(v);
}

void Enqueue::visit(AstArrayMem& v)
{
    // split off as new statement and variable earlier so nothing here

    // case of direct assignment to a LHS variable is handled earlier by
    // replacing the RHS AST array memory object with a variable
}

void Enqueue::visit(AstCond& v)
{
    // really this is like the C conditional ternary operator
    _rhsStmt << "(";
    v.getArg(0)->accept(*this); // predicate
    _rhsStmt << ") ? (";
    v.getArg(1)->accept(*this); // argument if predicate is true
    _rhsStmt << ") : (";
    v.getArg(2)->accept(*this); // argument if predicate is false
    _rhsStmt << ")";
}

void Enqueue::visit(AstConvert& v)
{
    // AST convert object is a NOP in the JIT
    // interpreter needs type of LHS for mixed precision assignment
    descendAst(v);
}

void Enqueue::visit(AstDotprod& v)
{
    // reduction statement requires multiple statements
    // AST object is used for the statement inside the accumulation loop
    // note nested reductions are impossible due to statement splits
    _rhsStmt << "(";
    v.getArg(0)->accept(*this); // left argument
    _rhsStmt << ") * (";
    v.getArg(1)->accept(*this); // right argument
    _rhsStmt << ")";
}

void Enqueue::visit(AstExtension& v)
{
    // split off as separate statement earlier
}

void Enqueue::visit(AstFun1& v)
{
    // function call with one argument
    _rhsStmt << v.fun() << "((";

    _rhsStmt << PrecType::getName(v.prec());

    if (_lhsVecLength > 1)
        _rhsStmt << _lhsVecLength;

    _rhsStmt << ")(";

    v.getArg(0)->accept(*this); // argument
    _rhsStmt << "))";
}

void Enqueue::visit(AstFun2& v)
{
    if (v.infix()) // overloaded operator with two arguments
    {
        const string opStr = v.fun();

        // relational operators return integers, convert back to floating point
        const bool isRelOp = ("==" == opStr ||
                              ">=" == opStr ||
                              ">"  == opStr ||
                              "<=" == opStr ||
                              "<"  == opStr ||
                              "!=" == opStr);

        if (isRelOp)
        {
            _rhsStmt << "((0 == ";
        }

        _rhsStmt << "(";
        v.getArg(0)->accept(*this); // left
        _rhsStmt << " "
                 << v.fun()
                 << " ";
        v.getArg(1)->accept(*this); // right
        _rhsStmt << ")";

        if (isRelOp)
        {
            _rhsStmt << ") ? ("
                     << PrecType::getName(v.prec());
            if (1 != _lhsVecLength) _rhsStmt << _lhsVecLength;
            _rhsStmt << ")(0) : ("
                     << PrecType::getName(v.prec());
            if (1 != _lhsVecLength) _rhsStmt << _lhsVecLength;
            _rhsStmt << ")(1))";
        }
    }
    else // function call with two arguments
    {
        _rhsStmt << v.fun() << "(";
        v.getArg(0)->accept(*this); // left
        _rhsStmt << ", ";
        v.getArg(1)->accept(*this); // right
        _rhsStmt << ")";
    }
}

void Enqueue::visit(AstFun3& v)
{
    // function call with three arguments
    _rhsStmt << v.fun() << "(";
    v.getArg(0)->accept(*this); // left
    _rhsStmt << ", ";
    v.getArg(1)->accept(*this); // middle
    _rhsStmt << ", ";
    v.getArg(2)->accept(*this); // right
    _rhsStmt << ")";
}

void Enqueue::visit(AstGather& v)
{
    if (v.eligible()) // gathering is eligible for texture sampling
    {
        const AstVariable* varPtr = v.dataVariable();
        const uint32_t varNum = varPtr->variable();

        const size_t vecLen = varPtr->isTraceVariable()
                                  ? getVecLength(varNum)
                                  : getVecLength(varPtr);

        const size_t effVecLen = 0 == vecLen
                                     ? PrecType::vecLength(v.prec())
                                     : vecLen;

        const size_t gatherVarNum = varPtr->isTraceVariable()
                                        ? _func->gatherVariable(
                                                     varNum,
                                                     effVecLen,
                                                     v.W(),
                                                     v.H() )
                                        : _func->gatherVariable(
                                                     varPtr,
                                                     effVecLen,
                                                     v.W(),
                                                     v.H() );

        vector< size_t > componentNums, subscriptNums;

        for (size_t i = 0; i < effVecLen; i++)
        {
            componentNums.push_back( (v.xOffset() + i) % effVecLen );

            subscriptNums.push_back( _func->gatherSubscript(
                                                gatherVarNum,
                                                v.N(),
                                                v.xHasIndex(),
                                                v.yHasIndex(),
                                                (v.xOffset() + i) % v.W(),
                                                v.yOffset() ) );
        }

        _rhsStmt << "(";

        _rhsStmt << PrecType::getName(v.prec());

        if (_lhsVecLength > 1)
            _rhsStmt << _lhsVecLength;

        _rhsStmt << ")(";

        for (size_t i = 0; i < effVecLen; i++)
        {
            const size_t subNum = subscriptNums[i];
            const size_t comNum = componentNums[i];

            Variable* subVar = _func->gatherVarFromSubscript(subNum);
            subVar->identifierName(_rhsStmt);
            _rhsStmt << subNum << ".s" << comNum;

            if (i < effVecLen - 1) _rhsStmt << ", ";
        }

        _rhsStmt << ")";
    }
    else              // gathering using vector length 1
    {
        // save RHS statement so far
        const string saveRHS = _rhsStmt.str();

        // width index
        _rhsStmt.str(string());
        v.getArg(1)->accept(*this);
        _subs.top()->setGatherWidthIndex(_rhsStmt.str());

        // height index
        if (2 == v.N())
        {
            _rhsStmt.str(string());
            v.getArg(2)->accept(*this);
            _subs.top()->setGatherHeightIndex(_rhsStmt.str());
        }

        // restore RHS
        _rhsStmt.str(string());
        _rhsStmt << saveRHS;

        // gather data using explicit width and height subscript
        v.getArg(0)->accept(*this);

        // restore the subscript to normal
        _subs.top()->unsetGatherIndex();
    }
}

void Enqueue::visit(AstIdxdata& v)
{
    if (_func->loopIndexes())
    {
        _rhsStmt << "(";

        _rhsStmt << PrecType::getName(v.prec());

        if (_lhsVecLength > 1)
            _rhsStmt << _lhsVecLength;

        _rhsStmt << ")(";

        stringstream ss;
        ss << "i" << v.index();

        switch (_lhsVecLength)
        {
            case (1) :
                _rhsStmt << ss.str();
                break;

            case (2) :
                _rhsStmt << ss.str() << " * " << _lhsVecLength << ", "
                         << ss.str() << " * " << _lhsVecLength << " + 1";
                break;

            case (4) :
                _rhsStmt << ss.str() << " * " << _lhsVecLength << ", "
                         << ss.str() << " * " << _lhsVecLength << " + 1, "
                         << ss.str() << " * " << _lhsVecLength << " + 2, "
                         << ss.str() << " * " << _lhsVecLength << " + 3";
                break;
        }

        _rhsStmt << ")";
    }
    else
    {
        _subs.top()->setVariable( v.W(),
                                  v.H(),
                                  _lhsVecLength );

        switch (_lhsVecLength)
        {
            case (1) :
                if (0 == v.index())
                {
                    _subs.top()->widthIdx(_rhsStmt);
                }
                else
                {
                    _subs.top()->disableBlocking();
                    _subs.top()->heightIdx(_rhsStmt);
                    _subs.top()->enableBlocking();
                }
                break;

            case (2) :
                _rhsStmt << "("
                         << PrecType::getName(v.prec())
                         << "2)(";
                if (0 == v.index())
                {
                    stringstream ss;
                    _subs.top()->widthIdx(ss);
                    ss << " * " << _lhsVecLength;
                    _rhsStmt << ss.str() << ", " << ss.str() << " + 1";
                }
                else
                {
                    _subs.top()->disableBlocking();
                    _subs.top()->heightIdx(_rhsStmt);
                    _subs.top()->enableBlocking();
                }
                _rhsStmt << ")";
                break;

            case (4) :
                _rhsStmt << "("
                         << PrecType::getName(v.prec())
                         << "4)(";

                if (0 == v.index())
                {
                    stringstream ss;
                    _subs.top()->widthIdx(ss);
                    ss << " * " << _lhsVecLength;
                    _rhsStmt << ss.str() << ", "
                             << ss.str() << " + 1,"
                             << ss.str() << " + 2,"
                             << ss.str() << " + 3";
                }
                else
                {
                    _subs.top()->disableBlocking();
                    _subs.top()->heightIdx(_rhsStmt);
                    _subs.top()->enableBlocking();
                }
                _rhsStmt << ")";
                break;
        }
    }
}

void Enqueue::visit(AstLitdata& v)
{
    if (1 != _lhsVecLength)
    {
        _rhsStmt << "("
                 << PrecType::getName(_lhsPrec)
                 << _lhsVecLength
                 << ")(";
    }

    switch (v.prec())
    {
        case (PrecType::UInt32) : _rhsStmt << v.uintValue(); break;
        case (PrecType::Int32) : _rhsStmt << v.intValue(); break;
        case (PrecType::Float) : _rhsStmt << v.floatValue(); break;
        case (PrecType::Double) : _rhsStmt << v.doubleValue(); break;
    }

    if (1 != _lhsVecLength)
    {
        _rhsStmt << ")";
    }
}

void Enqueue::visit(AstMakedata& v)
{
    // split off as separate statement earlier
}

void Enqueue::visit(AstMatmulMM& v)
{
    // split off as separate kernel earlier
}

void Enqueue::visit(AstMatmulMV& v)
{
    // split off as separate kernel earlier
}

void Enqueue::visit(AstMatmulVM& v)
{
    // split off as separate kernel earlier
}

void Enqueue::visit(AstMatmulVV& v)
{
    _rhsStmt << "(";

    _subs.top()->setOuterProductLeft();
    v.getArg(0)->accept(*this); // left argument

    _rhsStmt << ") * (";

    _subs.top()->setOuterProductRight();
    v.getArg(1)->accept(*this); // right argument

    _rhsStmt << ")";

    _subs.top()->unsetOuterProduct();
}

void Enqueue::visit(AstOpenCL& v)
{
    SourceKernel func(v.kernelName(),
                      v.programSource(),
                      v.programHashCode());

    const size_t numArgs = v.getNumArgs();

    for (size_t i = 0; i < numArgs; i++)
    {
        if (v.isArgArray(i))
        {
            func.pushArgArray(v.getArgPrec(i),
                              v.getArgUInt32(i));
        }
        else if (v.isArgLocal(i))
        {
            func.pushArgLocal(v.getArgPrec(i),
                              v.getArgSizeT(i));
        }
        else if (v.isArgScalar(i))
        {
            switch (v.getArgPrec(i))
            {
                case (PrecType::UInt32) :
                    func.pushArgScalar(v.getArgUInt32(i));
                    break;

                case (PrecType::Int32) :
                    func.pushArgScalar(v.getArgInt32(i));
                    break;

                case (PrecType::Float) :
                    func.pushArgScalar(v.getArgFloat(i));
                    break;

                case (PrecType::Double) :
                    func.pushArgScalar(v.getArgDouble(i));
                    break;
            }
        }
    }

    const size_t global0 = v.getGlobalWorkDim(0);
    const size_t global1 = v.getGlobalWorkDim(1);
    const size_t local0 = v.getLocalWorkDim(0);
    const size_t local1 = v.getLocalWorkDim(1);

    if (! _memMgr.enqueueKernel( _vt,
                                 func,
                                 global0,
                                 global1,
                                 local0,
                                 local1 ))
    {
        _failure = true;
    }
}

void Enqueue::visit(AstReadout& v)
{
    // split off as separate statement earlier
}

void Enqueue::visit(AstRNGnormal& v)
{
    _func->rngVecType(v.prec(),
                      _lhsVecLength);
    _func->rngVariant(v.rngVariant());

    _subs.top()->setVariable(v.W(), v.H(), _lhsVecLength);

    _rhsStmt << _func->rngNormal( v.prec(),
                                  _lhsVecLength,
                                  v.rngVariant(),
                                  v.rngSeed(),
                                  *_subs.top(),
                                  (_repeatIdx.empty()
                                       ? -1
                                       : _repeatIdx.top()) );
}

void Enqueue::visit(AstRNGuniform& v)
{
    _func->rngVecType(v.prec(),
                      _lhsVecLength);
    _func->rngVariant(v.rngVariant());

    _subs.top()->setVariable(v.W(), v.H(), _lhsVecLength);

    if (v.hasLimits())
        _rhsStmt << "(";

    _rhsStmt << _func->rngUniform( v.prec(),
                                   _lhsVecLength,
                                   v.rngVariant(),
                                   v.rngSeed(),
                                   *_subs.top(),
                                   (_repeatIdx.empty()
                                        ? -1
                                        : _repeatIdx.top()) );

    if (v.hasLimits())
    {
        const double intervalLength = v.maxLimit() - v.minLimit();
        const bool needShifting = v.minLimit() != 0.0;

        _rhsStmt << " * " << intervalLength;

        if (needShifting)
            _rhsStmt << " + " << v.minLimit();

        _rhsStmt << ")";
    }
}

void Enqueue::visit(AstScalar& v)
{
    if (1 != _lhsVecLength)
    {
        _rhsStmt << "("
                 << PrecType::getName(_lhsPrec)
                 << _lhsVecLength
                 << ")(";
    }

    switch (v.prec())
    {
        case (PrecType::UInt32) : _rhsStmt << v.uintValue(); break;
        case (PrecType::Int32) : _rhsStmt << v.intValue(); break;
        case (PrecType::Float) : _rhsStmt << v.floatValue(); break;
        case (PrecType::Double) : _rhsStmt << v.doubleValue(); break;
    }

    if (1 != _lhsVecLength)
    {
        _rhsStmt << ")";
    }
}

void Enqueue::visit(AstTranspose& v)
{
    // transpose the index space
    _subs.top()->setTranspose();

    descendAst(v);

    // un-transpose the index space
    _subs.top()->unsetTranspose();
}

void Enqueue::visit(AstVariable& v)
{
    Variable* funcVar = NULL;
    if (v.isTraceVariable())
    {
        const uint32_t varNum = v.variable();
        if (_xid->traceUseRegister().count(varNum))
        {
            funcVar = _func->traceVarPrivate(varNum);
        }
        else
        {
            funcVar = _func->traceVar(varNum);
        }
    }
    else
    {
        const AstVariable* varPtr = &v;
        funcVar = _func->splitVar(varPtr);
    }

    // same data across traces suppresses data tile part of subscript
    bool suppressTile = false;
    if (! _func->isPrivate(funcVar) && v.isTraceVariable())
    {
        const vector< size_t > frontIndex
                                   = _vt.memallocFrontIndex( v.variable() );

        set< size_t > frontSet;

        for (vector< size_t >::const_iterator
             it = frontIndex.begin(); it != frontIndex.end(); it++)
        {
            frontSet.insert(*it);
        }

        if (1 == frontSet.size())
        {
            suppressTile = true;
        }
    }

    const size_t vecLen = getVecLength(&v);

    // really the same hack as everywhere else for scalars, need to sum
    // vector elements for RHS 1x1 variables due to standard vector length
    // (note: only applying hack for global memory buffers)
    string reducePrefix, reduceSuffix;
    if (1 == v.H() && 1 == v.W() && 1 != vecLen && v.enableDotHack())
    {
        // standard vector length is double2 and float4
        reducePrefix = (2 == PrecType::vecLength(v.prec()))
                           ? "dot((double2)(1.0f), "
                           : "dot((float4)(1.0f), ";

        reduceSuffix = ")";
    }

    if (0 == vecLen) // image
    {
        const size_t effVecLen = PrecType::vecLength(v.prec());

        _subs.top()->setVariable( v.W(), v.H(), effVecLen );
        if (_subs.top()->isMixedZero())
        {
            _rhsStmt << "0";
        }
        else
        {
            stringstream ss;
            switch (v.prec())
            {
                case (PrecType::UInt32) :
                    ss << "read_imageui(";
                    break;

                case (PrecType::Int32) :
                    ss << "read_imagei(";
                    break;

                case (PrecType::Float) :
                    ss << "read_imagef(";
                    break;

                case (PrecType::Double) :
                    ss << "as_double2(read_imageui(";
                    break;
            }
            funcVar->identifierName(ss);
            ss << ", sampler, (int2)(";
            _subs.top()->widthIdx(ss);
            ss << ", ";
            if (suppressTile) _subs.top()->disableBlocking();
            _subs.top()->heightIdx(ss);
            if (suppressTile) _subs.top()->enableBlocking();
            switch (v.prec())
            {
                case (PrecType::UInt32) :
                    ss << "))";
                    break;

                case (PrecType::Int32) :
                    ss << "))";
                    break;

                case (PrecType::Float) :
                    ss << "))";
                    break;

                case (PrecType::Double) :
                    ss << ")))";
                    break;
            }
            if (_subs.top()->isMixedVecLength())
            {
                AddrMem* tempVar = _func->getPrivateVar( v.prec(),
                                                         effVecLen );
                _func->assignStatement(tempVar, ss.str());

                tempVar->identifierName(_rhsStmt);
                _subs.top()->mixedComponentSub(_rhsStmt);
            }
            else
            {
                _rhsStmt << ss.str();
            }
        }
    }
    else // memory buffer
    {
        if (_func->isPrivate(funcVar))
        {
            // private variables are scalars/vectors (but not arrays)
            funcVar->identifierName(_rhsStmt); // print variable identifier
        }
        else
        {
            // global variables are arrays and require a subscript
            _subs.top()->setVariable( v.W(), v.H(), vecLen );
            if (_subs.top()->isMixedZero())
            {
                _rhsStmt << "0";
            }
            else
            {
                if (! _scalarToScalar) _rhsStmt << reducePrefix;

                funcVar->identifierName(_rhsStmt); // print variable identifier

                if (suppressTile) _subs.top()->disableBlocking();
                _subs.top()->arraySub(_rhsStmt); // print array subscript
                if (suppressTile) _subs.top()->enableBlocking();

                if (! _scalarToScalar) _rhsStmt << reduceSuffix;
            }
        }
    }
}

}; // namespace chai_internal
