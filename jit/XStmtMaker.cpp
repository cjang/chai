// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

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
#include "AstReadout.hpp"
#include "AstRNGnormal.hpp"
#include "AstRNGuniform.hpp"
#include "AstScalar.hpp"
#include "AstTranspose.hpp"
#include "AstVariable.hpp"
#include "BaseAst.hpp"
#include "BCStmtCompound.hpp"
#include "BCStmtRepeat.hpp"
#include "BCStmtSingle.hpp"
#include "XStmtCompound.hpp"
#include "XStmtCreateData.hpp"
#include "XStmtExtension.hpp"
#include "XStmtExtensionAuto.hpp"
#include "XStmtIndex.hpp"
#include "XStmtLiteral.hpp"
#include "XStmtMaker.hpp"
#include "XStmtMatmul.hpp"
#include "XStmtMatmulAuto.hpp"
#include "XStmtReadData.hpp"
#include "XStmtReduce.hpp"
#include "XStmtRepeat.hpp"
#include "XStmtRNGrand.hpp"
#include "XStmtRNGseed.hpp"
#include "XStmtSendData.hpp"
#include "XStmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// transposed array variables

size_t XStmtMaker::getTransposeCount(void) const
{
    size_t transposeCount = 0;

    for (vector< AstTranspose* >::const_iterator
         it = _transposeStack.begin();
         it != _transposeStack.end();
         it++)
    {
        // transposed matmul arguments are absorbed into XGEMM
        if (0 == _trackMatmulArgs.count(*it))
            transposeCount++;
    }

    return transposeCount;
}

void XStmtMaker::insertTraceVar(const uint32_t varNum)
{
    // support for transposed array subscripts, force vector length to 1
    if (0 == _transposeTraceVar.count(varNum))
    {
        // number of containing transpose AST objects, excluding matmul()
        // arguments
        const size_t transposeCount = getTransposeCount();

        // odd number of transposes means variable is transposed
        if (1 == (transposeCount % 2))
            _transposeTraceVar.insert(varNum);
    }

    // support for gathered array subscripts, force vector length to 1
    if (! _gatherStack.empty())
        _gatherTraceVar.insert(varNum);
}

void XStmtMaker::insertSplitVar(AstVariable* varPtr)
{
    // support for transposed array subscripts, force vector length to 1
    if (0 == _transposeSplitVar.count(varPtr))
    {
        // number of containing transpose AST objects, excluding matmul()
        // arguments
        const size_t transposeCount = getTransposeCount();

        // odd number of transposes means variable is transposed
        if (1 == (transposeCount % 2))
            _transposeSplitVar.insert(varPtr);
    }

    // support for gathered array subscripts, force vector length to 1
    if (! _gatherStack.empty())
        _gatherSplitVar.insert(varPtr);
}

////////////////////////////////////////
// add new statements

void XStmtMaker::pushLexRoot(XStmt* s)
{
    _lexRoot.push_back(s);
}

void XStmtMaker::pushLexScope(XStmt* s)
{
    _lexScope.top()->push_back(s);

    s->transposeVars(_transposeTraceVar,
                     _transposeSplitVar);

    s->gatherVars(_gatherTraceVar,
                  _gatherSplitVar);
}

void XStmtMaker::pushLexScope(vector< XStmt* >* vs)
{
    _lexScope.push(vs);
}

void XStmtMaker::popLexScope(void)
{
    _lexScope.pop();
}

////////////////////////////////////////
// split statements for some operations
// and descent

void XStmtMaker::clearAst(void)
{
    _trackAst.clear();
    _trackIndex.clear();
    _trackExtension.clear();
    _trackLiteral.clear();
    _trackMatmul.clear();
    _trackMatmulArgs.clear(); // for moving transpose AST objects into kernel
    _trackReadData.clear();
    _trackReduce.clear();
    _trackRNG.clear();
    _trackSameDataAcrossTraces.clear();
    _trackSendData.clear();
    _trackTranspose.clear();
    _trackVar.clear();

    _transposeStack.clear();
    _transposeTraceVar.clear();
    _transposeSplitVar.clear();

    while (! _gatherStack.empty()) _gatherStack.pop();
    _gatherTraceVar.clear();
    _gatherSplitVar.clear();
}

void XStmtMaker::insertAst(BaseAst& v)
{
    _trackAst.insert(&v);
}

void XStmtMaker::insertExtension(BaseAst& v)
{
    _trackExtension.insert(&v);
}

void XStmtMaker::insertIndex(BaseAst& v)
{
    _trackIndex.insert(&v);
}

void XStmtMaker::insertLiteral(AstLitdata& v)
{
    _trackLiteral[&v] = LITERAL_DATA;
}

void XStmtMaker::insertLiteral(AstScalar& v)
{
    _trackLiteral[&v] = LITERAL_SCALAR;
}

void XStmtMaker::insertMatmul(AstMatmulMM& v)
{
    _trackMatmul[&v] = MATMUL_MM;

    _trackMatmulArgs.insert(v.getArg(0));
    _trackMatmulArgs.insert(v.getArg(1));
}

void XStmtMaker::insertMatmul(AstMatmulMV& v)
{
    _trackMatmul[&v] = MATMUL_MV;

    _trackMatmulArgs.insert(v.getArg(0));
    _trackMatmulArgs.insert(v.getArg(1));
}

void XStmtMaker::insertMatmul(AstMatmulVM& v)
{
    _trackMatmul[&v] = MATMUL_VM;

    _trackMatmulArgs.insert(v.getArg(0));
    _trackMatmulArgs.insert(v.getArg(1));
}

void XStmtMaker::insertReadData(AstReadout& v)
{
    _trackReadData.insert(&v);

    const AstVariable* varPtr
        = static_cast< const AstVariable* >( v.getArg(0) );

    _traceReadoutDim[ varPtr->variable() ] = v.readVarDim();
}

void XStmtMaker::insertReduce(AstAccum& v)
{
    _trackReduce[&v] = REDUCE_ACCUM;
}

void XStmtMaker::insertReduce(AstDotprod& v)
{
    _trackReduce[&v] = REDUCE_DOTPROD;
}

void XStmtMaker::insertRNG(AstRNGnormal& v)
{
    _trackRNG[&v] = RNG_NORMAL;
}

void XStmtMaker::insertRNG(AstRNGuniform& v)
{
    _trackRNG[&v] = RNG_UNIFORM;
}

void XStmtMaker::insertSameDataAcrossTraces(AstArrayMem& v)
{
    _trackSameDataAcrossTraces.insert(&v);
}

void XStmtMaker::insertSameDataAcrossTraces(AstVariable& v)
{
    _trackSameDataAcrossTraces.insert(&v);
}

void XStmtMaker::insertSendData(AstArrayMem& v)
{
    _trackSendData.insert(&v);
}

void XStmtMaker::insertSendData(AstMakedata& v)
{
    _trackSendData.insert(&v);
}

void XStmtMaker::insertTranspose(AstTranspose& v)
{
    _trackTranspose.insert(&v);
}

void XStmtMaker::insertVar(AstVariable& v)
{
    // used for RHS splits
    _trackVar.insert(&v);
}

void XStmtMaker::splitAst(BaseAst& v,
                          const size_t argIndex,
                          const SplitContext force)
{
    // send data splits
    if ( 0 != _trackSendData.count(v.getArg(argIndex)) )
    {
        // do not split if first assignment for the LHS variable
        if ( &v == _lhsConvert &&
             0 == _assignedVars.count(_lhsRoot->variable()) )
        {
            // assignment happens in visit(AstVariable&) function
            _assignedVars.insert(_lhsRoot->variable());

            return;
        }

        AstArrayMem* memObj = static_cast< AstArrayMem* >(v.getArg(argIndex));

        const uint32_t varNum = memObj->variable();
        const uint32_t varVersion = memObj->version();

        const pair< uint32_t, uint32_t> memKey
            = pair< uint32_t, uint32_t >(varNum, varVersion);

        if ( 0 == _memSplit.count(memKey) )
        {
            if (_vt.stickyMovement())
            {
                // must swizzle back to CPU host first
                memObj->backMem()->swizzle();

                // splitting off array memory as new statement and variable
                AstVariable* newVar = new AstVariable(memObj);

                // replace this argument with the new variable
                v.replaceArg(argIndex, newVar);

                // new send data statement
                XStmtSendData* datStmt = new XStmtSendData(newVar,
                                                           memObj,
                                                           false);

                // add into current list
                pushLexScope(datStmt);

                // remember the new variable for the split array memory
                _memSplit[memKey] = newVar;
            }
            else
            {
                AstVariable* varPtr = _vt.vectorNuts()[ varNum ]
                                         ->getNutVar( varVersion );

                // really direct assignment of RHS to LHS variable
                v.replaceArg(argIndex, varPtr);

                // remember the new variable for the split array memory
                _memSplit[memKey] = varPtr;
            }
        }
        else
        {
            // this array memory is already split off as a variable
            v.replaceArg(argIndex, _memSplit[memKey]);
        }

        // check for transposed array subscripts for this variable
        insertSplitVar(_memSplit[memKey]);
    }

    // operation splits
    else if (
              // if forced and the stream is not a plain old variable
              ((FORCE_EXTENSION == force || FORCE_MATMUL == force) &&
               0 == _trackVar.count(v.getArg(argIndex)))
                  || // OR
              // if stream is really a AST operation that we care about
              (0 != _trackAst.count(v.getArg(argIndex)))
            )
    {
        // do not split type conversions
        if ( (DEFAULT == force) && (&v == _lhsConvert) )
        {
            // assignment happens in visit(AstVariable&) function
            _assignedVars.insert(_lhsRoot->variable());

            return;
        }

        // do not split off the scalars alpha and beta from GEMM
        if ( FORCE_MATMUL == force &&
             0 != _trackMatmul.count(&v) &&
             argIndex > 2 ) // arg0=A, arg1=B, arg2=C, arg3=alpha, arg4=beta
        {
            return;
        }

        // the LHS split variable
        AstVariable* newVar = NULL;

        // special case of splitting GEMM, reuse C as split variable
        if ( 0 != _trackMatmul.count(v.getArg(argIndex)) )
        {
            AstMatmulBase* matmulBase = static_cast< AstMatmulBase* >(
                                            v.getArg(argIndex) );

            if (matmulBase->isGEMM())
            {
                // example: D = sum(matmul(A, B, C, 1.1, 2.2)) becomes
                // C = matmul(A, B, C, 1.1, 2.2)
                // D = sum(C)
                // there is no new split variable created in this case
                newVar = static_cast< AstVariable* >(
                             matmulBase->getArg(2) );
            }
        }

        // ...for all other cases besides splitting GEMM, need new variable
        XStmtCreateData* datStmt = NULL;
        if (NULL == newVar)
        {
            // create new variable
            newVar = new AstVariable(v.getArg(argIndex), true);

            // new create data statement
            datStmt = new XStmtCreateData( newVar,
                                           _repScope.empty() );

            // add into current list
            pushLexScope(datStmt);
        }

        if ( 0 != _trackExtension.count(v.getArg(argIndex)) )
        {
            // new language extension statement
            XStmtExtension* extStmt = new XStmtExtension(
                                              newVar,
                                              static_cast< AstExtension* >(
                                                  v.getArg(argIndex) ));

            // auto-tune request
            XStmtExtensionAuto* extAuto = new XStmtExtensionAuto(
                                                  newVar,
                                                  static_cast< AstExtension* >(
                                                      v.getArg(argIndex) ));

            // add into current list
            pushLexScope(extStmt);

            // request auto-tuned language extension kernel in cache
            pushLexRoot(extAuto);
        }
        else if ( 0 != _trackMatmul.count(v.getArg(argIndex)) )
        {
            // new matrix multiply statement
            XStmtMatmul* matStmt;

            // auto-tune request
            XStmtMatmulAuto* matAuto;

            switch (_trackMatmul[v.getArg(argIndex)])
            {
                case (MATMUL_MM) :
                    matStmt = new XStmtMatmul(
                                      newVar,
                                      static_cast< AstMatmulMM* >(
                                          v.getArg(argIndex) ),
                                      false);
                    matAuto = new XStmtMatmulAuto(
                                      newVar,
                                      static_cast< AstMatmulMM* >(
                                          v.getArg(argIndex) ));
                    break;

                case (MATMUL_MV) :
                    matStmt = new XStmtMatmul(
                                      newVar,
                                      static_cast< AstMatmulMV* >(
                                          v.getArg(argIndex) ),
                                      false);
                    matAuto = new XStmtMatmulAuto(
                                      newVar,
                                      static_cast< AstMatmulMV* >(
                                          v.getArg(argIndex) ));
                    break;

                case (MATMUL_VM) :
                    matStmt = new XStmtMatmul(
                                      newVar,
                                      static_cast< AstMatmulVM* >(
                                          v.getArg(argIndex) ),
                                      false);
                    matAuto = new XStmtMatmulAuto(
                                      newVar,
                                      static_cast< AstMatmulVM* >(
                                          v.getArg(argIndex) ));
                    break;
            }

            // add into current list
            pushLexScope(matStmt);

            // request auto-tuned matrix multiply kernel in cache
            pushLexRoot(matAuto);
        }
        else if ( 0 != _trackReduce.count(v.getArg(argIndex)) )
        {
            // new reduction to scalar statement
            XStmtReduce* redStmt;
            switch (_trackReduce[v.getArg(argIndex)])
            {
                case (REDUCE_ACCUM) :
                    redStmt = new XStmtReduce(
                                      newVar,
                                      static_cast< AstAccum* >(
                                          v.getArg(argIndex) ),
                                      false);
                    break;

                case (REDUCE_DOTPROD) :
                    redStmt = new XStmtReduce(
                                      newVar,
                                      static_cast< AstDotprod* >(
                                          v.getArg(argIndex) ),
                                      false);
                    break;
            }

            // add into current list
            pushLexScope(redStmt);

            // private reduction variable vector length comes from RHS
            if (datStmt)
            {
                datStmt->underlyingVars(redStmt->rhsVariable());
            }
        }
        else
        {
            // new single statement
            XStmtSingle* sinStmt = new XStmtSingle(newVar);

            // add into current list
            pushLexScope(sinStmt);
        }

        // replace this argument with the new variable
        v.replaceArg(argIndex, newVar);

        // check for transposed array subscripts for this variable
        insertSplitVar(newVar);
    }
}

void XStmtMaker::splitAst(BaseAst& v,
                          const SplitContext force)
{
    // check all AST stream arguments
    for (size_t i = 0; i < v.numArg(); i++)
        splitAst(v, i, force);
}

void XStmtMaker::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

////////////////////////////////////////
// create converted trace

XStmtMaker::XStmtMaker(vector< XStmt* >& lexRoot,
                       VectorTrace& vt)
    : _vt(vt),
      _lexRoot(lexRoot),
      _lexScope(),
      _lhsRoot(NULL),
      _lhsConvert(NULL),
      _reductionInsideLoop(false)
{
    pushLexScope(&lexRoot);
}

const map< uint32_t, size_t >& XStmtMaker::traceReadoutDim(void) const
{
    return _traceReadoutDim;
}

bool XStmtMaker::reductionInsideLoop(void) const
{
    return _reductionInsideLoop;
}

////////////////////////////////////////
// VisitBCStmt

void XStmtMaker::visit(BCStmtCompound& s)
{
    // new compound statement
    XStmtCompound* comStmt = new XStmtCompound;

    // add to outer repeat statement
    _repScope.top()->setCompound(comStmt);

    // push compound statement list context
    pushLexScope(&comStmt->stuffInside());

    // visit each statement inside compound
    for (vector< BCStmt* >::const_iterator
         it = s.stuffInside().begin();
         it != s.stuffInside().end();
         it++)
    {
        (*it)->accept(*this);
    }

    // pop compound statement list context
    popLexScope();
}

void XStmtMaker::visit(BCStmtRepeat& s)
{
    // new repeat statement
    XStmtRepeat* repStmt = new XStmtRepeat(s.numReps());

    // add new repeat statement into current list
    pushLexScope(repStmt);

    // push repeat statement
    _repScope.push(repStmt);

    // visit statement
    s.stuffInside()->accept(*this);

    // pop repeat statement
    _repScope.pop();
}

void XStmtMaker::visit(BCStmtSingle& s)
{
    // constructor or destructor have empty RHS, skip these
    if (NULL == s.getAst())
        return;

    // must descend into AST RHS tree
    _lhsRoot = NULL;
    _lhsConvert = NULL;

    s.getAst()->accept(*this);
}

////////////////////////////////////////
// VisitAst

void XStmtMaker::visit(AstAccum& v)
{
    insertAst(v); // reduction: mean, sum
    insertReduce(v);
    descendAst(v);
    splitAst(v);
    if (! _repScope.empty())
        _reductionInsideLoop = true;
}

void XStmtMaker::visit(AstArrayMem& v)
{
    insertSendData(v);

    if (v.sameDataAcrossTraces())
        insertSameDataAcrossTraces(v);
}

void XStmtMaker::visit(AstCond& v)
{
    descendAst(v);
    splitAst(v);
}

void XStmtMaker::visit(AstConvert& v)
{
    descendAst(v);
    _lhsConvert = &v; // to limit array memory splits
    splitAst(v);
}

void XStmtMaker::visit(AstDotprod& v)
{
    insertAst(v); // reduction: dot_product
    insertReduce(v);
    descendAst(v);
    splitAst(v);
    if (! _repScope.empty())
        _reductionInsideLoop = true;
}

void XStmtMaker::visit(AstExtension& v)
{
    insertAst(v); // language extension is always separate kernel
    insertExtension(v);
    descendAst(v);

    splitAst(v, FORCE_EXTENSION);
    if (! _repScope.empty())
        _repScope.top()->setLoopRoll(false);
}

void XStmtMaker::visit(AstFun1& v)
{
    descendAst(v);
    splitAst(v);
}

void XStmtMaker::visit(AstFun2& v)
{
    descendAst(v);
    splitAst(v);
}

void XStmtMaker::visit(AstFun3& v)
{
    descendAst(v);
    splitAst(v);
}

void XStmtMaker::visit(AstGather& v)
{
    _gatherStack.push(&v);

    descendAst(v);
    splitAst(v);

    _gatherStack.pop();
}

void XStmtMaker::visit(AstIdxdata& v)
{
    insertIndex(v);
}

void XStmtMaker::visit(AstLitdata& v)
{
    insertLiteral(v);
}

void XStmtMaker::visit(AstMakedata& v)
{
    insertSendData(v);

    // assignment to LHS variable
    _assignedVars.insert(_lhsRoot->variable());

    // makeN_fxy() always generates a new array variable. It never appears
    // inside a RHS expression. An enclosing transpose() is not possible.
}

void XStmtMaker::visit(AstMatmulMM& v)
{
    insertAst(v); // reduction: special case of XGEMM
    insertMatmul(v);
    descendAst(v);

    // incorporate transposed A argument into matmul AST object
    if (_trackTranspose.count(v.getArg(0)))
    {
        v.setTransposeA();
        BaseAst* tmpArg = v.getArg(0);
        v.replaceArg(0, tmpArg->getArg(0));
        delete tmpArg->release();
    }

    // incorporate transposed B argument into matmul AST object
    if (_trackTranspose.count(v.getArg(1)))
    {
        v.setTransposeB();
        BaseAst* tmpArg = v.getArg(1);
        v.replaceArg(1, tmpArg->getArg(0));
        delete tmpArg->release();
    }

    // check for same data across traces for A argument
    if (_trackSameDataAcrossTraces.count(v.getArg(0)))
    {
        v.setSameDataA();
    }

    // check for same data across traces for B argument
    if (_trackSameDataAcrossTraces.count(v.getArg(1)))
    {
        v.setSameDataB();
    }

    // preparation for using optimized XGEMM library kernel
    splitAst(v, FORCE_MATMUL);
    if (! _repScope.empty())
        _repScope.top()->setLoopRoll(false);
}

void XStmtMaker::visit(AstMatmulMV& v)
{
    insertAst(v); // reduction: matmul matrix*vector lifted to XGEMM
    insertMatmul(v);
    descendAst(v);

    // incorporate transposed A argument into matmul AST object
    if (_trackTranspose.count(v.getArg(0)))
    {
        v.setTransposeA();
        BaseAst* tmpArg = v.getArg(0);
        v.replaceArg(0, tmpArg->getArg(0));
        delete tmpArg->release();
    }

    // incorporate transposed B argument into matmul AST object
    if (_trackTranspose.count(v.getArg(1)))
    {
        v.setTransposeB();
        BaseAst* tmpArg = v.getArg(1);
        v.replaceArg(1, tmpArg->getArg(0));
        delete tmpArg->release();
    }

    // check for same data across traces for A argument
    if (_trackSameDataAcrossTraces.count(v.getArg(0)))
    {
        v.setSameDataA();
    }

    // check for same data across traces for B argument
    if (_trackSameDataAcrossTraces.count(v.getArg(1)))
    {
        v.setSameDataB();
    }

    // preparation for using optimized XGEMM library kernel
    splitAst(v, FORCE_MATMUL);
    if (! _repScope.empty())
        _repScope.top()->setLoopRoll(false);
}

void XStmtMaker::visit(AstMatmulVM& v)
{
    insertAst(v); // reduction: matmul vector*matrix lifted to XGEMM
    insertMatmul(v);
    descendAst(v);

    // incorporate transposed A argument into matmul AST object
    if (_trackTranspose.count(v.getArg(0)))
    {
        v.setTransposeA();
        BaseAst* tmpArg = v.getArg(0);
        v.replaceArg(0, tmpArg->getArg(0));
        delete tmpArg->release();
    }

    // incorporate transposed B argument into matmul AST object
    if (_trackTranspose.count(v.getArg(1)))
    {
        v.setTransposeB();
        BaseAst* tmpArg = v.getArg(1);
        v.replaceArg(1, tmpArg->getArg(0));
        delete tmpArg->release();
    }

    // check for same data across traces for A argument
    if (_trackSameDataAcrossTraces.count(v.getArg(0)))
    {
        v.setSameDataA();
    }

    // check for same data across traces for B argument
    if (_trackSameDataAcrossTraces.count(v.getArg(1)))
    {
        v.setSameDataB();
    }

    // preparation for using optimized XGEMM library kernel
    splitAst(v, FORCE_MATMUL);
    if (! _repScope.empty())
        _repScope.top()->setLoopRoll(false);
}

void XStmtMaker::visit(AstMatmulVV& v)
{
    // outer product is inlined in kernels

    AstTranspose outerProductLeft(v.getArg(0));

    _transposeStack.push_back(&outerProductLeft);
    v.getArg(0)->accept(*this); // left argument is really transposed
    splitAst(v, 0);
    _transposeStack.pop_back();

    v.getArg(1)->accept(*this);
    splitAst(v, 1);
}

void XStmtMaker::visit(AstReadout& v)
{
    insertReadData(v);
}

void XStmtMaker::visit(AstRNGnormal& v)
{
    insertRNG(v);

    // assignment to LHS variable
    _assignedVars.insert(_lhsRoot->variable());
}

void XStmtMaker::visit(AstRNGuniform& v)
{
    insertRNG(v);

    // assignment to LHS variable
    _assignedVars.insert(_lhsRoot->variable());
}

void XStmtMaker::visit(AstScalar& v)
{
    insertLiteral(v);
}

void XStmtMaker::visit(AstTranspose& v)
{
    insertTranspose(v);

    _transposeStack.push_back(&v);

    descendAst(v);
    splitAst(v);

    _transposeStack.pop_back();
}

void XStmtMaker::visit(AstVariable& v)
{
    if (NULL == _lhsRoot)
    {
        // this is the top-most LHS AST variable

        // all other variables are on the RHS
        _lhsRoot = &v;

        clearAst();

        // continue descent
        descendAst(v);

        XStmtCreateData* createData = NULL;
        if ( 0 == _lhsTraceVariables.count(v.variable()) )
        {
            // new create data statement
            createData = new XStmtCreateData(&v, _repScope.empty());

            // remember this variable so it's not created twice
            _lhsTraceVariables.insert(v.variable());
        }

        // special case handing of scalar to scalar variable assignment
        bool scalarToScalar = false;

        // continuation and scheduling between devices
        if ( v.getArg(0) == _lhsConvert &&
             0 != _trackSendData.count(_lhsConvert->getArg(0)) )
        {
            scalarToScalar = true;

            AstArrayMem* memObj = static_cast< AstArrayMem* >(
                                      _lhsConvert->getArg(0) );

            const uint32_t varNum = memObj->variable();
            const uint32_t varVersion = memObj->version();

            if (_vt.stickyMovement())
            {
                // must swizzle back to CPU host first
                memObj->backMem()->swizzle();

                // new send data statement
                XStmtSendData* datStmt
                    = new XStmtSendData(&v, memObj, ! _lhsConvert->isNOP());

                // add into current list
                pushLexScope(datStmt);

                // no need to create as send data does that already
                delete createData;
                createData = NULL;
            }
            else
            {
                AstVariable* varPtr = _vt.vectorNuts()[ varNum ]
                                         ->getNutVar( varVersion );

                // really direct assignment of RHS to LHS variable
                _lhsConvert->replaceArg(0, varPtr);
            }
        }

        // different kinds of statement possible
        if ( 0 != _trackSendData.count(v.getArg(0)) )
        {
            AstMakedata* makObj = static_cast< AstMakedata* >(v.getArg(0));

            // new send data statement
            XStmtSendData* datStmt = new XStmtSendData(&v,
                                                       makObj,
                                                       false);

            // add into current list
            pushLexScope(datStmt);

            // no need to create as send data does that already
            delete createData;
        }
        else if ( 0 != _trackReadData.count(v.getArg(0)) )
        {
            AstReadout* redObj = static_cast< AstReadout* >(v.getArg(0));

            // new read data statement
            XStmtReadData* datStmt = new XStmtReadData(&v,
                                                       redObj);

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(datStmt);
        }
        else if ( v.getArg(0) == _lhsConvert &&
                  0 != _trackExtension.count(_lhsConvert->getArg(0)) )
        {
            // new language extension statement
            XStmtExtension* extStmt = new XStmtExtension(
                                              &v,
                                              static_cast< AstExtension* >(
                                                  _lhsConvert->getArg(0) ));

            // auto-tune request
            XStmtExtensionAuto* extAuto = new XStmtExtensionAuto(
                                                  &v,
                                                  static_cast< AstExtension* >(
                                                      _lhsConvert->getArg(0) ));

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(extStmt);

            // request auto-tuned language extension kernel in cache
            pushLexRoot(extAuto);
        }
        else if ( v.getArg(0) == _lhsConvert &&
                  0 != _trackMatmul.count(_lhsConvert->getArg(0)) )
        {
            // new matrix multiply statement
            XStmtMatmul* matStmt;

            // auto-tune request
            XStmtMatmulAuto* matAuto;

            switch (_trackMatmul[_lhsConvert->getArg(0)])
            {
                case (MATMUL_MM) :
                    matStmt = new XStmtMatmul(
                                      &v,
                                      static_cast< AstMatmulMM* >(
                                          _lhsConvert->getArg(0) ),
                                      ! _lhsConvert->isNOP());
                    matAuto = new XStmtMatmulAuto(
                                      &v,
                                      static_cast< AstMatmulMM* >(
                                          _lhsConvert->getArg(0) ));
                    break;

                case (MATMUL_MV) :
                    matStmt = new XStmtMatmul(
                                      &v,
                                      static_cast< AstMatmulMV* >(
                                          _lhsConvert->getArg(0) ),
                                      ! _lhsConvert->isNOP());
                    matAuto = new XStmtMatmulAuto(
                                      &v,
                                      static_cast< AstMatmulMV* >(
                                          _lhsConvert->getArg(0) ));
                    break;

                case (MATMUL_VM) :
                    matStmt = new XStmtMatmul(
                                      &v,
                                      static_cast< AstMatmulVM* >(
                                          _lhsConvert->getArg(0) ),
                                      ! _lhsConvert->isNOP());
                    matAuto = new XStmtMatmulAuto(
                                      &v,
                                      static_cast< AstMatmulVM* >(
                                          _lhsConvert->getArg(0) ));
                    break;
            }

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(matStmt);

            // request auto-tuned matrix multiply kernel in cache
            pushLexRoot(matAuto);
        }
        else if ( v.getArg(0) == _lhsConvert &&
                  0 != _trackReduce.count(_lhsConvert->getArg(0)) )
        {
            // new reduction to scalar statement
            XStmtReduce* redStmt;
            switch (_trackReduce[_lhsConvert->getArg(0)])
            {
                case (REDUCE_ACCUM) :
                    redStmt = new XStmtReduce(
                                      &v,
                                      static_cast< AstAccum* >(
                                          _lhsConvert->getArg(0) ),
                                      ! _lhsConvert->isNOP());
                    break;

                case (REDUCE_DOTPROD) :
                    redStmt = new XStmtReduce(
                                      &v,
                                      static_cast< AstDotprod* >(
                                          _lhsConvert->getArg(0) ),
                                      ! _lhsConvert->isNOP());
                    break;
            }

            // add into current list
            if (createData)
            {
                pushLexScope(createData);

                // private variable dimensions for the reduction loop
                createData->underlyingVars(redStmt->rhsVariable());
            }
            pushLexScope(redStmt);
        }
        else if ( 0 != _trackRNG.count(v.getArg(0)) )
        {
            // new random number generator statement
            XStmtRNGrand* rngStmt;
            switch (_trackRNG[v.getArg(0)])
            {
                case (RNG_NORMAL) :
                    rngStmt = new XStmtRNGrand(
                                      &v,
                                      static_cast< AstRNGnormal* >(
                                          v.getArg(0) ));
                    break;

                case (RNG_UNIFORM) :
                    rngStmt = new XStmtRNGrand(
                                      &v,
                                      static_cast< AstRNGuniform* >(
                                          v.getArg(0) ));
                    break;
            }

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(rngStmt);

            // new random seed statement
            const size_t seedLen = v.W();
            if (0 == _rngSeeds.count(seedLen))
            {
                _rngSeeds.insert(seedLen);
                pushLexRoot(new XStmtRNGseed(seedLen));
            }
        }
        else if ( 0 != _trackLiteral.count(v.getArg(0)) )
        {
            // There is an issue here. Literal data and scalars are not the
            // same. A scalar is immutable. Data may or may not be mutated.
            // If the variable corresponding to the literal data appears on
            // the LHS, then it is mutable and should be array data. If the
            // variable appears on the RHS only, then it is safe to treat
            // the same as a scalar.
            // ...However, there is another issue. A variable may appear
            // on the RHS only in one trace and on the LHS in another.

            // new declare literal statement
            XStmtLiteral* litStmt;
            switch (_trackLiteral[v.getArg(0)])
            {
                case (LITERAL_DATA) :
                    litStmt = new XStmtLiteral(
                                      &v,
                                      static_cast< AstLitdata* >(
                                          v.getArg(0) ));
                    break;

                case (LITERAL_SCALAR) :
                    litStmt = new XStmtLiteral(
                                      &v,
                                      static_cast< AstScalar* >(
                                          v.getArg(0) ));
                    break;
            }

            // add into current list
            if (createData)
            {
                // Literal data AST objects correspond to ones_fXX and
                // zeros_fXX declarations. Scalar AST objects correspond to
                // literal constants (floats and doubles). However, for
                // creating compute device memory objects, they are treated
                // identically (for now). For example, there is no difference
                // between:
                //
                //     Arrayf64 x = ones_f64(1);
                //     Arrayf64 x = 1;

                if (AstLitdata* litDat = litStmt->litdataPtr())
                {
                    if (litDat->isDouble())
                    {
                        createData->initializeData(litDat->doubleValue());
                    }
                    else
                    {
                        createData->initializeData(litDat->floatValue());
                    }
                }

                if (AstScalar* scaDat = litStmt->scalarPtr())
                {
                    if (scaDat->isDouble())
                    {
                        createData->initializeData(scaDat->doubleValue());
                    }
                    else
                    {
                        createData->initializeData(scaDat->floatValue());
                    }
                }

                pushLexScope(createData);
            }
            pushLexScope(litStmt);
        }
        else if ( 0 != _trackIndex.count(v.getArg(0)) )
        {
            // AST index objects do not directly imply new array data.
            //
            // Creates a new 10x10 array variable C.
            //
            //     C = index_f32(0, 10, 10);
            //     C = dx * index_f32(0, 10, 10);
            //
            // Creates a new 1x1 array variable C, really a scalar.
            //
            //     C = sum(exp(index_f32(0, 10, 10)));
            //
            // Note that in no case is there any data created on the CPU host
            // and sent over the bus to the GPU.

            AstIdxdata* idxObj = static_cast< AstIdxdata* >(v.getArg(0));

            // new index statement
            XStmtIndex* idxStmt = new XStmtIndex(&v,
                                                 idxObj);

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(idxStmt);
        }
        else
        {
            // new single statement
            XStmtSingle* sinStmt = new XStmtSingle(&v);

            // special case handing of scalar to scalar variable assignment
            const bool hasOneArg = 1 == v.numArg();
            const bool varCaseA = 0 != _trackVar.count(v.getArg(0));
            const bool varCaseB = v.getArg(0) == _lhsConvert &&
                                  0 != _trackVar.count(_lhsConvert->getArg(0));
            if (hasOneArg && (varCaseA || varCaseB))
            {
                // statement is variable to variable
                if (1 == v.H() && 1 == v.W()) // LHS is a scalar
                {
                    scalarToScalar = true;
                }
            }

            // pure heuristic hack: disable dot() by setting scalarToScalar
            // if only one variable appears on LHS and RHS sides, this is
            // to handle cases like: A += 1 as it is the same as: A = A + 1
            if (! scalarToScalar)
            {
                if (1 == _trackVar.size())
                {
                    AstVariable* rhsVar
                        = static_cast< AstVariable* >(*_trackVar.begin());

                    if (v.isTraceVariable() &&
                        rhsVar->isTraceVariable() &&
                        v.variable() == rhsVar->variable())
                    {
                        scalarToScalar = true;
                    }
                }
            }
            sinStmt->scalarToScalar(scalarToScalar);

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(sinStmt);
        }
    }
    else
    {
        // RHS variable
        insertVar(v);

        if (v.isSameDataAcrossTraces())
            insertSameDataAcrossTraces(v);

        // check if this variable requires transposed array subscript
        insertTraceVar(v.variable());
    }
}

}; // namespace chai_internal
