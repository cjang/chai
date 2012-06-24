// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "ByteCodes.hpp"
#include "EditStak.hpp"
#include "HashJIT.hpp"
#include "BCStmtSingle.hpp"
#include "PrintBC.hpp"
#include "TEA.hpp"

using namespace std;

namespace chai_internal {

uint64_t BCStmtSingle::computeHash(VectorTrace& vt)
{
    HashJIT v;

    v.push(_lhsVariable);
    //v.push(_lhsVersion); // loop rolling will not work if version included

    // for RNG loop rolling, hash the seed argument with a zero
    EditStak rngEdit;
    rngEdit.nullifyOpArg(ByteCodes::rng_normal_make_u32, 1);
    rngEdit.nullifyOpArg(ByteCodes::rng_normal_make_i32, 1);
    rngEdit.nullifyOpArg(ByteCodes::rng_normal_make_f32, 1);
    rngEdit.nullifyOpArg(ByteCodes::rng_normal_make_f64, 1);
    rngEdit.nullifyOpArg(ByteCodes::rng_uniform_make_u32, 1);
    rngEdit.nullifyOpArg(ByteCodes::rng_uniform_make_i32, 1);
    rngEdit.nullifyOpArg(ByteCodes::rng_uniform_make_f32, 1);
    rngEdit.nullifyOpArg(ByteCodes::rng_uniform_make_f64, 1);
    _rhsBytecode.descend(rngEdit);
    rngEdit.getBytecode().descend(v);

    if (-1 != v.ptrIndex())
    {
        const size_t stmtIndex = v.ptrIndex();

        _frontMem = vt.frontMem()[stmtIndex];

        _backMem = vt.backMem().count(stmtIndex)
                       ? vt.backMem()[stmtIndex]
                       : NULL;
    }
    else
    {
        _backMem = NULL;
    }

    for (set< uint32_t >::const_iterator
         it = v.variables().begin();
         it != v.variables().end();
         it++)
    {
        _vectorNuts[*it] = vt.vectorNuts()[*it];
    }

    _vectorNuts[_lhsVariable] = vt.vectorNuts()[_lhsVariable];

    return v.code();
}

BCStmtSingle::BCStmtSingle(const size_t stmtIndex,
                           VectorTrace& vt)
    : _lhsVariable(vt.getVariable(stmtIndex)),
      _lhsVersion(vt.getVersion(stmtIndex)),
      _rhsBytecode(vt.getRHS(stmtIndex)),
      _lhsVariableIsLive(vt.liveVariables().count(_lhsVariable)),
      _constructorLHS(false),
      _destructorLHS(false),
      _frontMem(),
      _backMem(NULL),
      _vectorNuts(),
      _hashCode(computeHash(vt)) { }

BCStmtSingle::BCStmtSingle(const size_t stmtIndex,
                           VectorTrace& vt,
                           const Stak<BC>& rhsBytecodeOverride)
    : _lhsVariable(vt.getVariable(stmtIndex)),
      _lhsVersion(vt.getVersion(stmtIndex)),
      _rhsBytecode(rhsBytecodeOverride),
      _lhsVariableIsLive(vt.liveVariables().count(_lhsVariable)),
      _constructorLHS(false),
      _destructorLHS(false),
      _frontMem(),
      _backMem(NULL),
      _vectorNuts(),
      _hashCode(computeHash(vt)) { }

BCStmtSingle::BCStmtSingle(const size_t stmtIndex,
                           VectorTrace& vt,
                           const uint32_t lhsVariableOverride)
    : _lhsVariable(lhsVariableOverride),
      _lhsVersion(vt.getVersion(stmtIndex)),
      _rhsBytecode(vt.getRHS(stmtIndex)),
      _lhsVariableIsLive(vt.liveVariables().count(_lhsVariable)),
      _constructorLHS(false),
      _destructorLHS(false),
      _frontMem(),
      _backMem(NULL),
      _vectorNuts(),
      _hashCode(computeHash(vt)) { }

BCStmtSingle::BCStmtSingle(const size_t stmtIndex,
                           VectorTrace& vt,
                           const uint32_t lhsVariableOverride,
                           const Stak<BC>& rhsBytecodeOverride)
    : _lhsVariable(lhsVariableOverride),
      _lhsVersion(vt.getVersion(stmtIndex)),
      _rhsBytecode(rhsBytecodeOverride),
      _lhsVariableIsLive(vt.liveVariables().count(_lhsVariable)),
      _constructorLHS(false),
      _destructorLHS(false),
      _frontMem(),
      _backMem(NULL),
      _vectorNuts(),
      _hashCode(computeHash(vt)) { }

uint32_t BCStmtSingle::lhsVariable(void) const
{
    return _lhsVariable;
}

uint32_t BCStmtSingle::lhsVersion(void) const
{
    return _lhsVersion;
}

const Stak<BC>& BCStmtSingle::rhsBytecode(void) const
{
    return _rhsBytecode;
}

bool BCStmtSingle::lhsVariableIsLive(void) const
{
    return _lhsVariableIsLive;
}

bool BCStmtSingle::getConstructorLHS(void) const
{
    return _constructorLHS;
}

bool BCStmtSingle::getDestructorLHS(void) const
{
    return _destructorLHS;
}

void BCStmtSingle::setConstructorLHS(void)
{
    _constructorLHS = true;
}

void BCStmtSingle::setDestructorLHS(void)
{
    _destructorLHS = true;
}

vector< FrontMem* >& BCStmtSingle::frontMem(void)
{
    return _frontMem;
}

const vector< FrontMem* >& BCStmtSingle::frontMem(void) const
{
    return _frontMem;
}

BackMem* BCStmtSingle::backMem(void) const
{
    return _backMem;
}

uint64_t BCStmtSingle::hashCode(void)
{
    return _hashCode;
}

AstVariable* BCStmtSingle::getAst(void)
{
    return _vectorNuts[_lhsVariable]->getNutVar(_lhsVersion);
}

void BCStmtSingle::setAst(AstVariable* obj)
{
    // assign to LHS
    _vectorNuts[_lhsVariable]->setNutVar(_lhsVersion, obj);
}

void BCStmtSingle::debug(ostream& os)
{
    os << "Single(" << _hashCode << ")\t";

    os << _lhsVariable << " <- ";

    PrintBC v(os, " ");
    _rhsBytecode.descend(v);

    if (! _frontMem.empty())
    {
        os << "( ";

        for (vector< FrontMem* >::const_iterator
             it = _frontMem.begin();
             it != _frontMem.end();
             it++)
        {
            os << (*it) << " ";
        }

        os << ")";
    }

    os << endl;
}

void BCStmtSingle::accept(VisitBCStmt& v)
{
    v.visit(*this);
}

}; // namespace chai_internal
