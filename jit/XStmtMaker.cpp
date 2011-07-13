// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "AstAccum.hpp"
#include "AstArrayMem.hpp"
#include "AstBinop.hpp"
#include "AstCond.hpp"
#include "AstConvert.hpp"
#include "AstDotprod.hpp"
#include "AstGather.hpp"
#include "AstIdxdata.hpp"
#include "AstIsomorph.hpp"
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
#include "AstVariable.hpp"
#include "BCStmtCompound.hpp"
#include "BCStmtRepeat.hpp"
#include "BCStmtSingle.hpp"
#include "XStmtCompound.hpp"
#include "XStmtCreateData.hpp"
#include "XStmtGather.hpp"
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

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// add new statements

void XStmtMaker::pushLexRoot(XStmt* s)
{
    _lexRoot.push_back(s);
}

void XStmtMaker::pushLexScope(XStmt* s)
{
    _lexScope.top()->push_back(s);
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
    _trackGather.clear();
    _trackIndex.clear();
    _trackLiteral.clear();
    _trackMatmul.clear();
    _trackReadData.clear();
    _trackReduce.clear();
    _trackRNG.clear();
    _trackSendData.clear();
    _trackVar.clear();
}

void XStmtMaker::insertAst(BaseAst& v)
{
    _trackAst.insert(&v);
}

void XStmtMaker::insertGather(BaseAst& v)
{
    _trackGather.insert(&v);
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
}

void XStmtMaker::insertMatmul(AstMatmulMV& v)
{
    _trackMatmul[&v] = MATMUL_MV;
}

void XStmtMaker::insertMatmul(AstMatmulVM& v)
{
    _trackMatmul[&v] = MATMUL_VM;
}

void XStmtMaker::insertReadData(AstReadout& v)
{
    _trackReadData.insert(&v);
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

void XStmtMaker::insertSendData(AstArrayMem& v)
{
    _trackSendData.insert(&v);
}

void XStmtMaker::insertSendData(AstMakedata& v)
{
    _trackSendData.insert(&v);
}

void XStmtMaker::insertVar(AstVariable& v)
{
    // used for RHS splits
    _trackVar.insert(&v);
}

void XStmtMaker::splitAst(BaseAst& v, const SplitContext force)
{
    // check all AST stream arguments
    for (size_t i = 0; i < v.numArg(); i++)
    {
        // send data splits
        if ( 0 != _trackSendData.count(v.getArg(i)) )
        {
            // do not split if first assignment for the LHS variable
            if ( &v == _lhsConvert &&
                 0 == _assignedVars.count(_lhsRoot->variable()) )
            {
                // assignment happens in visit(AstVariable&) function
                _assignedVars.insert(_lhsRoot->variable());

                break;
            }

            AstArrayMem* memObj = static_cast< AstArrayMem* >(v.getArg(i));

            const pair< uint32_t, uint32_t> memKey
                = pair< uint32_t, uint32_t >(memObj->variable(),
                                             memObj->version());

            if ( 0 == _memSplit.count(memKey) )
            {
                // splitting off array memory as new statement and variable
                AstVariable* newVar = new AstVariable(memObj);

                // split array memory variable is new and will never appear
                // on the LHS anywhere else - therefore it is read-only
                // (use image textures for matrix multiply arguments)
                if (FORCE_MATMUL == force)
                {
                    newVar->setReadOnly();
                }

                // replace this argument with the new variable
                v.replaceArg(i, newVar);

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
                // this array memory is already split off as a variable
                v.replaceArg(i, _memSplit[memKey]);
            }
        }

        // operation splits
        else if (
                  // if forced and the stream is not a plain old variable
                  (FORCE_MATMUL == force && 0 == _trackVar.count(v.getArg(i)))
                      || // OR
                  // if stream is really a AST operation that we care about
                  (0 != _trackAst.count(v.getArg(i)))
                )
        {
            // do not split type conversions
            if ( (DEFAULT == force) && (&v == _lhsConvert) )
            {
                // assignment happens in visit(AstVariable&) function
                _assignedVars.insert(_lhsRoot->variable());

                break;
            }

            // create new variable
            AstVariable* newVar = new AstVariable(v.getArg(i));

            // new create data statement
            XStmtCreateData* datStmt = new XStmtCreateData(newVar);

            // add into current list
            pushLexScope(datStmt);

            if ( 0 != _trackGather.count(v.getArg(i)) )
            {
                // new gather statement
                XStmtGather* gatStmt = new XStmtGather(
                                               newVar,
                                               static_cast< AstGather* >(
                                                   v.getArg(i) ),
                                               false);

                // add into current list
                pushLexScope(gatStmt);
            }
            else if ( 0 != _trackMatmul.count(v.getArg(i)) )
            {
                // new matrix multiply statement
                XStmtMatmul* matStmt;
                switch (_trackMatmul[v.getArg(i)])
                {
                    case (MATMUL_MM) :
                        matStmt = new XStmtMatmul(
                                          newVar,
                                          static_cast< AstMatmulMM* >(
                                              v.getArg(i) ),
                                          false);
                        break;

                    case (MATMUL_MV) :
                        matStmt = new XStmtMatmul(
                                          newVar,
                                          static_cast< AstMatmulMV* >(
                                              v.getArg(i) ),
                                          false);
                        break;

                    case (MATMUL_VM) :
                        matStmt = new XStmtMatmul(
                                          newVar,
                                          static_cast< AstMatmulVM* >(
                                              v.getArg(i) ),
                                          false);
                        break;
                }

                // add into current list
                pushLexScope(matStmt);
            }
            else if ( 0 != _trackReduce.count(v.getArg(i)) )
            {
                // new reduction to scalar statement
                XStmtReduce* redStmt;
                switch (_trackReduce[v.getArg(i)])
                {
                    case (REDUCE_ACCUM) :
                        redStmt = new XStmtReduce(
                                          newVar,
                                          static_cast< AstAccum* >(
                                              v.getArg(i) ),
                                          false);
                        break;

                    case (REDUCE_DOTPROD) :
                        redStmt = new XStmtReduce(
                                          newVar,
                                          static_cast< AstDotprod* >(
                                              v.getArg(i) ),
                                          false);
                        break;
                }

                // add into current list
                pushLexScope(redStmt);
            }
            else
            {
                // new single statement
                XStmtSingle* sinStmt = new XStmtSingle(newVar);

                // add into current list
                pushLexScope(sinStmt);
            }

            // replace this argument with the new variable
            v.replaceArg(i, newVar);
        }
    }
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

XStmtMaker::XStmtMaker(vector< XStmt* >& lexRoot)
    : _lexRoot(lexRoot),
      _lexScope(),
      _lhsRoot(NULL),
      _lhsConvert(NULL)
{
    pushLexScope(&lexRoot);
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
}

void XStmtMaker::visit(AstArrayMem& v)
{
    insertSendData(v);
}

void XStmtMaker::visit(AstBinop& v)
{
    descendAst(v);
    splitAst(v);
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
}

void XStmtMaker::visit(AstGather& v)
{
    insertAst(v); // potential uncoalesced memory access patterns
    insertGather(v);
    descendAst(v);
    splitAst(v);
}

void XStmtMaker::visit(AstIdxdata& v)
{
    insertIndex(v);
}

void XStmtMaker::visit(AstIsomorph& v)
{
    descendAst(v);
    splitAst(v);
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
}

void XStmtMaker::visit(AstMatmulMM& v)
{
    insertAst(v); // reduction: special case of XGEMM
    insertMatmul(v);
    descendAst(v);

    // preparation for using optimized XGEMM library kernel
    splitAst(v, FORCE_MATMUL);
    if (! _repScope.empty()) _repScope.top()->setLoopRoll(false);
}

void XStmtMaker::visit(AstMatmulMV& v)
{
    insertAst(v); // reduction: matmul matrix*vector lifted to XGEMM
    insertMatmul(v);
    descendAst(v);

    // preparation for using optimized XGEMM library kernel
    splitAst(v, FORCE_MATMUL);
    if (! _repScope.empty()) _repScope.top()->setLoopRoll(false);
}

void XStmtMaker::visit(AstMatmulVM& v)
{
    insertAst(v); // reduction: matmul vector*matrix lifted to XGEMM
    insertMatmul(v);
    descendAst(v);

    // preparation for using optimized XGEMM library kernel
    splitAst(v, FORCE_MATMUL);
    if (! _repScope.empty()) _repScope.top()->setLoopRoll(false);
}

void XStmtMaker::visit(AstMatmulVV& v)
{
    descendAst(v);
    splitAst(v);
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
            createData = new XStmtCreateData(&v);

            // remember this variable so it's not created twice
            _lhsTraceVariables.insert(v.variable());
        }

        // different kinds of statement possible
        if ( v.getArg(0) == _lhsConvert &&
             0 != _trackSendData.count(_lhsConvert->getArg(0)) )
        {
            AstArrayMem* memObj = static_cast< AstArrayMem* >(
                                      _lhsConvert->getArg(0) );

            // new send data statement
            XStmtSendData* datStmt = new XStmtSendData(&v,
                                                       memObj,
                                                       ! _lhsConvert->isNOP());

            // no need to create as send data does that already
            delete createData;

            // add into current list
            pushLexScope(datStmt);
        }
        else if ( 0 != _trackSendData.count(v.getArg(0)) )
        {
            AstMakedata* makObj = static_cast< AstMakedata* >(v.getArg(0));

            // new send data statement
            XStmtSendData* datStmt = new XStmtSendData(&v,
                                                       makObj,
                                                       false);

            // no need to create as send data does that already
            delete createData;

            // add into current list
            pushLexScope(datStmt);
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
                  0 != _trackMatmul.count(_lhsConvert->getArg(0)) )
        {
            // new matrix multiply statement
            XStmtMatmul* matStmt;
            switch (_trackMatmul[_lhsConvert->getArg(0)])
            {
                case (MATMUL_MM) :
                    matStmt = new XStmtMatmul(
                                      &v,
                                      static_cast< AstMatmulMM* >(
                                          _lhsConvert->getArg(0) ),
                                      ! _lhsConvert->isNOP());
                    break;

                case (MATMUL_MV) :
                    matStmt = new XStmtMatmul(
                                      &v,
                                      static_cast< AstMatmulMV* >(
                                          _lhsConvert->getArg(0) ),
                                      ! _lhsConvert->isNOP());
                    break;

                case (MATMUL_VM) :
                    matStmt = new XStmtMatmul(
                                      &v,
                                      static_cast< AstMatmulVM* >(
                                          _lhsConvert->getArg(0) ),
                                      ! _lhsConvert->isNOP());
                    break;
            }

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(matStmt);
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
            if (createData) pushLexScope(createData);
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
            // FIXME - There is a problem here. Literal data and scalars are
            // not the same. A scalar is immutable. Data may or may not be
            // mutated. If the variable corresponding to the literal data
            // appears on the LHS, then it is mutable and should be array
            // data. If the variable appears on the RHS only, then it is safe
            // to treat the same as a scalar.
            // ...However, there is another problem. A variable may appear
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
            AstIdxdata* idxObj = static_cast< AstIdxdata* >(v.getArg(0));

            // new index statement
            XStmtIndex* idxStmt = new XStmtIndex(&v,
                                                 idxObj);

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(idxStmt);
        }
        else if ( v.getArg(0) == _lhsConvert &&
                  0 != _trackGather.count(_lhsConvert->getArg(0)) )
        {
            AstGather* gatObj = static_cast< AstGather* >(
                                    _lhsConvert->getArg(0) );

            // new gather statement
            XStmtGather* gatStmt = new XStmtGather(&v,
                                                   gatObj,
                                                   ! _lhsConvert->isNOP());
               
            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(gatStmt);
        }
        else
        {
            // new single statement
            XStmtSingle* sinStmt = new XStmtSingle(&v);

            // add into current list
            if (createData) pushLexScope(createData);
            pushLexScope(sinStmt);
        }
    }
    else
    {
        // RHS variable
        insertVar(v);
    }
}

}; // namespace chai_internal
