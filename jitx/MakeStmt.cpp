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
#include "MakeStmt.hpp"
#include "PrecType.hpp"
#include "StmtCompound.hpp"
#include "StmtCreateData.hpp"
#include "StmtExtension.hpp"
#include "StmtExtensionAuto.hpp"
#include "StmtGatherAuto.hpp"
#include "StmtIndex.hpp"
#include "StmtLiteral.hpp"
#include "StmtMatmul.hpp"
#include "StmtMatmulAuto.hpp"
#include "StmtReadData.hpp"
#include "StmtReduce.hpp"
#include "StmtRepeat.hpp"
#include "StmtSendData.hpp"
#include "StmtSingle.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// transposed array variables

size_t MakeStmt::getTransposeCount(void) const
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

void MakeStmt::specialTrackVar(void)
{
    if ( 1 == (getTransposeCount() % 2) || // odd number means transposed
         ! _gatherStack.empty() )          // ineligible gather
    {
        _scalarVectorLength = true; // force vector length to 1
    }
}

////////////////////////////////////////
// add new statements

void MakeStmt::pushLexRoot(Stmt* s)
{
    _lexRoot.push_back(s);
}

void MakeStmt::pushLexScope(Stmt* s)
{
    _lexScope.top()->push_back(s);

    s->scalarVectorLength(_scalarVectorLength);
}

void MakeStmt::pushLexScope(vector< Stmt* >* vs)
{
    _lexScope.push(vs);
}

void MakeStmt::popLexScope(void)
{
    _lexScope.pop();
}

////////////////////////////////////////
// split statements for some operations
// and descent

void MakeStmt::clearAst(void)
{
    _trackAst.clear();
    _trackExtension.clear();
    _trackIndex.clear();
    _trackLiteral.clear();
    _trackMatmul.clear();
    _trackMatmulArgs.clear(); // for moving transpose AST objects into kernel
    _trackReadData.clear();
    _trackReduce.clear();
    _trackSameDataAcrossTraces.clear();
    _trackSendData.clear();
    _trackTranspose.clear();
    _trackVar.clear();

    _transposeStack.clear();
    while (! _gatherStack.empty()) _gatherStack.pop();
    _scalarVectorLength = false;

/*FIXME - remove this
    while (! _writebackStack.empty()) _writebackStack.pop();
*/
}

void MakeStmt::insertAst(BaseAst& v)
{
    _trackAst.insert(&v);
}

void MakeStmt::insertExtension(AstExtension& v)
{
    _trackExtension.insert(&v);
}

void MakeStmt::insertIndex(AstIdxdata& v)
{
    _trackIndex.insert(&v);
}

void MakeStmt::insertLiteral(AstLitdata& v)
{
    _trackLiteral[&v] = LITERAL_DATA;
}

void MakeStmt::insertLiteral(AstScalar& v)
{
    _trackLiteral[&v] = LITERAL_SCALAR;
}

void MakeStmt::insertMatmul(AstMatmulMM& v)
{
    _trackMatmul[&v] = MATMUL_MM;

    _trackMatmulArgs.insert(v.getArg(0));
    _trackMatmulArgs.insert(v.getArg(1));
}

void MakeStmt::insertMatmul(AstMatmulMV& v)
{
    _trackMatmul[&v] = MATMUL_MV;

    _trackMatmulArgs.insert(v.getArg(0));
    _trackMatmulArgs.insert(v.getArg(1));
}

void MakeStmt::insertMatmul(AstMatmulVM& v)
{
    _trackMatmul[&v] = MATMUL_VM;

    _trackMatmulArgs.insert(v.getArg(0));
    _trackMatmulArgs.insert(v.getArg(1));
}

void MakeStmt::insertReadData(AstReadout& v)
{
    _trackReadData.insert(&v);

    const AstVariable* varPtr
        = static_cast< const AstVariable* >( v.getArg(0) );

    _traceReadoutDim[ varPtr->variable() ] = v.readVarDim();
}

void MakeStmt::insertReduce(AstAccum& v)
{
    _trackReduce[&v] = REDUCE_ACCUM;
}

void MakeStmt::insertReduce(AstDotprod& v)
{
    _trackReduce[&v] = REDUCE_DOTPROD;
}

void MakeStmt::insertSameDataAcrossTraces(AstArrayMem& v)
{
    _trackSameDataAcrossTraces.insert(&v);
}

void MakeStmt::insertSameDataAcrossTraces(AstVariable& v)
{
    _trackSameDataAcrossTraces.insert(&v);
}

void MakeStmt::insertSendData(AstArrayMem& v)
{
    _trackSendData.insert(&v);
}

void MakeStmt::insertSendData(AstMakedata& v)
{
    _trackSendData.insert(&v);
}

void MakeStmt::insertTranspose(AstTranspose& v)
{
    _trackTranspose.insert(&v);
}

void MakeStmt::insertVar(AstVariable& v)
{
    // used for RHS splits
    _trackVar.insert(&v);
}

void MakeStmt::splitAst(BaseAst& v,
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
                StmtSendData* datStmt = new StmtSendData(newVar, memObj);

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

        // check for transposed and gathered array subscripts
        specialTrackVar();
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
        StmtCreateData* datStmt = NULL;
        if (NULL == newVar)
        {
            // create new variable
            newVar = new AstVariable(v.getArg(argIndex), true);

            // new create data statement
            datStmt = new StmtCreateData( newVar,
                                          _repScope.empty() );

            // add into current list
            pushLexScope(datStmt);
        }

        if ( 0 != _trackExtension.count(v.getArg(argIndex)) )
        {
            // new language extension statement
            StmtExtension* extStmt = new StmtExtension(
                                             newVar,
                                             static_cast< AstExtension* >(
                                                 v.getArg(argIndex) ));

            // auto-tune request
            StmtExtensionAuto* extAuto = new StmtExtensionAuto(
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
            StmtMatmul* matStmt;

            // auto-tune request
            StmtMatmulAuto* matAuto;

            switch (_trackMatmul[v.getArg(argIndex)])
            {
                case (MATMUL_MM) :
                    matStmt = new StmtMatmul(
                                      newVar,
                                      static_cast< AstMatmulMM* >(
                                          v.getArg(argIndex) ) );
                    matAuto = new StmtMatmulAuto(
                                      newVar,
                                      static_cast< AstMatmulMM* >(
                                          v.getArg(argIndex) ));
                    break;

                case (MATMUL_MV) :
                    matStmt = new StmtMatmul(
                                      newVar,
                                      static_cast< AstMatmulMV* >(
                                          v.getArg(argIndex) ) );
                    matAuto = new StmtMatmulAuto(
                                      newVar,
                                      static_cast< AstMatmulMV* >(
                                          v.getArg(argIndex) ));
                    break;

                case (MATMUL_VM) :
                    matStmt = new StmtMatmul(
                                      newVar,
                                      static_cast< AstMatmulVM* >(
                                          v.getArg(argIndex) ) );
                    matAuto = new StmtMatmulAuto(
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
            StmtReduce* redStmt;

            switch (_trackReduce[v.getArg(argIndex)])
            {
                case (REDUCE_ACCUM) :
                    redStmt = new StmtReduce(
                                      newVar,
                                      static_cast< AstAccum* >(
                                          v.getArg(argIndex) ) );
                    break;

                case (REDUCE_DOTPROD) :
                    redStmt = new StmtReduce(
                                      newVar,
                                      static_cast< AstDotprod* >(
                                          v.getArg(argIndex) ) );
                    break;
            }

            newVar->disableDotHack();

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
            StmtSingle* sinStmt = new StmtSingle(newVar);

            // add into current list
            pushLexScope(sinStmt);
        }

        // replace this argument with the new variable
        v.replaceArg(argIndex, newVar);

        // check for transposed and gathered array subscripts
        specialTrackVar();
    }
}

void MakeStmt::splitAst(BaseAst& v,
                        const SplitContext force)
{
    // check all AST stream arguments
    for (size_t i = 0; i < v.numArg(); i++)
        splitAst(v, i, force);
}

void MakeStmt::descendAst(BaseAst& v)
{
    for (size_t i = 0; i < v.numArg(); i++)
    {
        v.getArg(i)->accept(*this);
    }
}

////////////////////////////////////////
// create converted trace

MakeStmt::MakeStmt(vector< Stmt* >& lexRoot,
                   VectorTrace& vt)
    : _vt(vt),
      _lexRoot(lexRoot),
      _lexScope(),
      _lhsRoot(NULL),
      _lhsConvert(NULL),
      _constructorLHS(false),
      _destructorLHS(false),
      _reductionInsideLoop(false)
{
    pushLexScope(&lexRoot);
}

const map< uint32_t, size_t >& MakeStmt::traceReadoutDim(void) const
{
    return _traceReadoutDim;
}

bool MakeStmt::reductionInsideLoop(void) const
{
    return _reductionInsideLoop;
}

////////////////////////////////////////
// VisitBCStmt

void MakeStmt::visit(BCStmtCompound& s)
{
    // new compound statement
    StmtCompound* comStmt = new StmtCompound;

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

void MakeStmt::visit(BCStmtRepeat& s)
{
    // new repeat statement
    StmtRepeat* repStmt = new StmtRepeat(s.numReps());

    // add new repeat statement into current list
    pushLexScope(repStmt);

    // push repeat statement
    _repScope.push(repStmt);

    // visit statement
    s.stuffInside()->accept(*this);

    // pop repeat statement
    _repScope.pop();
}

void MakeStmt::visit(BCStmtSingle& s)
{
    // constructor or destructor have empty RHS, skip these
    if (NULL == s.getAst())
        return;

    // must descend into AST RHS tree
    _lhsRoot = NULL;
    _lhsConvert = NULL;
    _constructorLHS = s.getConstructorLHS();
    _destructorLHS = s.getDestructorLHS();

    s.getAst()->accept(*this);
}

////////////////////////////////////////
// VisitAst

void MakeStmt::visit(AstAccum& v)
{
    insertAst(v); // reduction: mean, sum
    insertReduce(v);

/*FIXME - remove this
    _writebackStack.push(&v);
*/

    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

    splitAst(v);
    if (! _repScope.empty())
        _reductionInsideLoop = true;

/*FIXME - remove this
    for (set< AstVariable* >::const_iterator
         it = v.rhsVariable().begin();
         it != v.rhsVariable().end();
         it++)
    {
        if ((*it)->getValueFromRNG() && (*it)->getForceWriteback())
        {
            v.setNotTogether();
            break;
        }
    }
*/
}

void MakeStmt::visit(AstArrayMem& v)
{
    insertSendData(v);

    if (v.sameDataAcrossTraces())
        insertSameDataAcrossTraces(v);
}

void MakeStmt::visit(AstCond& v)
{
    descendAst(v);

    splitAst(v);
}

void MakeStmt::visit(AstConvert& v)
{
    descendAst(v);

    _lhsConvert = &v; // to limit array memory splits
    splitAst(v);
}

void MakeStmt::visit(AstDotprod& v)
{
    insertAst(v); // reduction: dot_product
    insertReduce(v);

/*FIXME - remove this
    _writebackStack.push(&v);
*/
    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

    splitAst(v);
    if (! _repScope.empty())
        _reductionInsideLoop = true;
}

void MakeStmt::visit(AstExtension& v)
{
    insertAst(v); // language extension is always separate kernel
    insertExtension(v);

/*FIXME - remove this
    _writebackStack.push(&v);
*/

    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

    splitAst(v, FORCE_EXTENSION);
    if (! _repScope.empty())
        _repScope.top()->setLoopRoll(false);
}

void MakeStmt::visit(AstFun1& v)
{
    descendAst(v);

    splitAst(v);
}

void MakeStmt::visit(AstFun2& v)
{
    descendAst(v);

    splitAst(v);
}

void MakeStmt::visit(AstFun3& v)
{
    descendAst(v);

    splitAst(v);
}

void MakeStmt::visit(AstGather& v)
{
    if (! v.eligible())
    {
        _gatherStack.push(&v);
    }
    else
    {
        pushLexRoot(
            new StmtGatherAuto(v.dataVariable()) );
    }

/*FIXME - remove this
    _writebackStack.push(&v);
*/

    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

    splitAst(v);

    if (! v.eligible())
        _gatherStack.pop();
}

void MakeStmt::visit(AstIdxdata& v)
{
    insertIndex(v);
}

void MakeStmt::visit(AstLitdata& v)
{
    insertLiteral(v);
}

void MakeStmt::visit(AstMakedata& v)
{
    insertSendData(v);

    // assignment to LHS variable
    _assignedVars.insert(_lhsRoot->variable());

    // makeN_fxy() always generates a new array variable. It never appears
    // inside a RHS expression. An enclosing transpose() is not possible.
}

void MakeStmt::visit(AstMatmulMM& v)
{
    insertAst(v); // reduction: special case of XGEMM
    insertMatmul(v);

/*FIXME - remove this
    _writebackStack.push(&v);
*/

    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

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

void MakeStmt::visit(AstMatmulMV& v)
{
    insertAst(v); // reduction: matmul matrix*vector lifted to XGEMM
    insertMatmul(v);

/*FIXME - remove this
    _writebackStack.push(&v);
*/

    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

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

void MakeStmt::visit(AstMatmulVM& v)
{
    insertAst(v); // reduction: matmul vector*matrix lifted to XGEMM
    insertMatmul(v);

/*FIXME - remove this
    _writebackStack.push(&v);
*/

    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

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

void MakeStmt::visit(AstMatmulVV& v)
{
    // outer product is inlined in kernels

    AstTranspose outerProductLeft(v.getArg(0));

    _transposeStack.push_back(&outerProductLeft);

/*FIXME - remove this
    _writebackStack.push(&v);
*/

    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

    v.getArg(0)->accept(*this); // left argument is really transposed
    splitAst(v, 0);

    _transposeStack.pop_back();

    v.getArg(1)->accept(*this);
    splitAst(v, 1);
}

void MakeStmt::visit(AstReadout& v)
{
    insertReadData(v);
}

void MakeStmt::visit(AstRNGnormal& v)
{
/*FIXME - remove this
    _lhsRoot->setValueFromRNG();
*/

/*FIXME - remove this
    if (! _repScope.empty())
    {
        // RNG is inside a rolled loop
        v.setInsideRolledLoop();
    }
*/
}

void MakeStmt::visit(AstRNGuniform& v)
{
/*FIXME - remove this
    _lhsRoot->setValueFromRNG();
*/

/*FIXME - remove this
    if (! _repScope.empty())
    {
        // RNG is inside a rolled loop
        v.setInsideRolledLoop();
    }
*/
}

void MakeStmt::visit(AstScalar& v)
{
    insertLiteral(v);
}

void MakeStmt::visit(AstTranspose& v)
{
    insertTranspose(v);

    _transposeStack.push_back(&v);

/*FIXME - remove this
    _writebackStack.push(&v);
*/

    descendAst(v);

/*FIXME - remove this
    _writebackStack.pop();
*/

    splitAst(v);

    _transposeStack.pop_back();
}

void MakeStmt::visit(AstVariable& v)
{
    if (NULL == _lhsRoot)
    {
        // this is the top-most LHS AST variable

        // all other variables are on the RHS
        _lhsRoot = &v;

        clearAst();

        // continue descent
        descendAst(v);

        StmtCreateData* createData = NULL;
        if ( 0 == _lhsTraceVariables.count(v.variable()) )
        {
            // new create data statement
            createData = new StmtCreateData(&v, _repScope.empty());
            if (_constructorLHS) createData->setConstructorLHS();
            if (_destructorLHS) createData->setDestructorLHS();

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
                StmtSendData* datStmt = new StmtSendData(&v, memObj);
                if (_constructorLHS) datStmt->setConstructorLHS();
                if (_destructorLHS) datStmt->setDestructorLHS();

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
            StmtSendData* datStmt = new StmtSendData(&v, makObj);
            if (_constructorLHS) datStmt->setConstructorLHS();
            if (_destructorLHS) datStmt->setDestructorLHS();

            // add into current list
            pushLexScope(datStmt);

            // no need to create as send data does that already
            delete createData;
        }
        else if ( 0 != _trackReadData.count(v.getArg(0)) )
        {
            AstReadout* redObj = static_cast< AstReadout* >(v.getArg(0));

            // new read data statement
            StmtReadData* datStmt = new StmtReadData(&v,
                                                     redObj);
            if (_constructorLHS) datStmt->setConstructorLHS();
            if (_destructorLHS) datStmt->setDestructorLHS();

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(datStmt);
        }
        else if ( v.getArg(0) == _lhsConvert &&
                  0 != _trackExtension.count(_lhsConvert->getArg(0)) )
        {
            // new language extension statement
            StmtExtension* extStmt = new StmtExtension(
                                             &v,
                                             static_cast< AstExtension* >(
                                                 _lhsConvert->getArg(0) ));
            if (_constructorLHS) extStmt->setConstructorLHS();
            if (_destructorLHS) extStmt->setDestructorLHS();

            // auto-tune request
            StmtExtensionAuto* extAuto = new StmtExtensionAuto(
                                                 &v,
                                                 static_cast< AstExtension* >(
                                                     _lhsConvert->getArg(0) ));
            if (_constructorLHS) extAuto->setConstructorLHS();
            if (_destructorLHS) extAuto->setDestructorLHS();

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
            StmtMatmul* matStmt;

            // auto-tune request
            StmtMatmulAuto* matAuto;

            switch (_trackMatmul[_lhsConvert->getArg(0)])
            {
                case (MATMUL_MM) :
                    matStmt = new StmtMatmul(
                                      &v,
                                      static_cast< AstMatmulMM* >(
                                          _lhsConvert->getArg(0) ) );
                    if (_constructorLHS) matStmt->setConstructorLHS();
                    if (_destructorLHS) matStmt->setDestructorLHS();
                    matAuto = new StmtMatmulAuto(
                                      &v,
                                      static_cast< AstMatmulMM* >(
                                          _lhsConvert->getArg(0) ));
                    if (_constructorLHS) matAuto->setConstructorLHS();
                    if (_destructorLHS) matAuto->setDestructorLHS();
                    break;

                case (MATMUL_MV) :
                    matStmt = new StmtMatmul(
                                      &v,
                                      static_cast< AstMatmulMV* >(
                                          _lhsConvert->getArg(0) ) );
                    if (_constructorLHS) matStmt->setConstructorLHS();
                    if (_destructorLHS) matStmt->setDestructorLHS();
                    matAuto = new StmtMatmulAuto(
                                      &v,
                                      static_cast< AstMatmulMV* >(
                                          _lhsConvert->getArg(0) ));
                    if (_constructorLHS) matAuto->setConstructorLHS();
                    if (_destructorLHS) matAuto->setDestructorLHS();
                    break;

                case (MATMUL_VM) :
                    matStmt = new StmtMatmul(
                                      &v,
                                      static_cast< AstMatmulVM* >(
                                          _lhsConvert->getArg(0) ) );
                    if (_constructorLHS) matStmt->setConstructorLHS();
                    if (_destructorLHS) matStmt->setDestructorLHS();
                    matAuto = new StmtMatmulAuto(
                                      &v,
                                      static_cast< AstMatmulVM* >(
                                          _lhsConvert->getArg(0) ));
                    if (_constructorLHS) matAuto->setConstructorLHS();
                    if (_destructorLHS) matAuto->setDestructorLHS();
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
            StmtReduce* redStmt;
            switch (_trackReduce[_lhsConvert->getArg(0)])
            {
                case (REDUCE_ACCUM) :
                    redStmt = new StmtReduce(
                                      &v,
                                      static_cast< AstAccum* >(
                                          _lhsConvert->getArg(0) ) );
                    if (_constructorLHS) redStmt->setConstructorLHS();
                    if (_destructorLHS) redStmt->setDestructorLHS();
                    break;

                case (REDUCE_DOTPROD) :
                    redStmt = new StmtReduce(
                                      &v,
                                      static_cast< AstDotprod* >(
                                          _lhsConvert->getArg(0) ) );
                    if (_constructorLHS) redStmt->setConstructorLHS();
                    if (_destructorLHS) redStmt->setDestructorLHS();
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
            StmtLiteral* litStmt;
            switch (_trackLiteral[v.getArg(0)])
            {
                case (LITERAL_DATA) :
                    litStmt = new StmtLiteral(
                                      &v,
                                      static_cast< AstLitdata* >(
                                          v.getArg(0) ));
                    if (_constructorLHS) litStmt->setConstructorLHS();
                    if (_destructorLHS) litStmt->setDestructorLHS();
                    break;

                case (LITERAL_SCALAR) :
                    litStmt = new StmtLiteral(
                                      &v,
                                      static_cast< AstScalar* >(
                                          v.getArg(0) ));
                    if (_constructorLHS) litStmt->setConstructorLHS();
                    if (_destructorLHS) litStmt->setDestructorLHS();
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
                    switch (litDat->precision())
                    {
                        case (PrecType::UInt32) :
                            createData->initializeData(litDat->uintValue());
                            break;
                        case (PrecType::Int32) :
                            createData->initializeData(litDat->intValue());
                            break;
                        case (PrecType::Float) :
                            createData->initializeData(litDat->floatValue());
                            break;
                        case (PrecType::Double) :
                            createData->initializeData(litDat->doubleValue());
                            break;
                    }
                }

                if (AstScalar* scaDat = litStmt->scalarPtr())
                {
                    switch (scaDat->precision())
                    {
                        case (PrecType::UInt32) :
                            createData->initializeData(scaDat->uintValue());
                            break;
                        case (PrecType::Int32) :
                            createData->initializeData(scaDat->intValue());
                            break;
                        case (PrecType::Float) :
                            createData->initializeData(scaDat->floatValue());
                            break;
                        case (PrecType::Double) :
                            createData->initializeData(scaDat->doubleValue());
                            break;
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
            StmtIndex* idxStmt = new StmtIndex(&v,
                                               idxObj);
            if (_constructorLHS) idxStmt->setConstructorLHS();
            if (_destructorLHS) idxStmt->setDestructorLHS();

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(idxStmt);
        }
        else
        {
            // new single statement
            StmtSingle* sinStmt = new StmtSingle(&v);
            if (_constructorLHS) sinStmt->setConstructorLHS();
            if (_destructorLHS) sinStmt->setDestructorLHS();

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

        // check for transposed and gathered array subscripts
        specialTrackVar();

/*FIXME - remove this
        if (! _writebackStack.empty())
        {
            v.setForceWriteback();
        }
*/
    }
}

}; // namespace chai_internal
