// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "Function.hpp"
#include "OCLhacks.hpp"
#include "PrecType.hpp"
#include "Qualifier.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// compute device function

string Function::makeName(const uint64_t hashCode,
                          const size_t kernelNumber) const
{
    stringstream ss;
    ss << "f_" << hashCode << "_" << kernelNumber;
    return ss.str();
}

void Function::addTraceArgument(const uint32_t variable, Variable* obj)
{
    _arguments.push_back(obj);
    _varTrace[obj] = variable;
    _traceVar[variable] = obj;

    _argumentLookup[obj] = _argumentLookup.size();
    obj->setFunction(this);
}

void Function::addSplitArgument(const AstVariable* variable, Variable* obj)
{
    _arguments.push_back(obj);
    _varSplit[obj] = variable;
    _splitVar[variable] = obj;

    _argumentLookup[obj] = _argumentLookup.size();
    obj->setFunction(this);
}

void Function::addPrivate(AddrMem* obj)
{
    if (obj->fp64()) _fp64 = true;
    _typeAddrMem.insert(obj);
    _privates.push_back(obj);

    _privateLookup[obj] = _privateLookup.size();
    obj->setFunction(this);
}

void Function::addPrivate(Integer* obj)
{
    _typeInteger.insert(obj);
    _privates.push_back(obj);

    _privateLookup[obj] = _privateLookup.size();
    obj->setFunction(this);
}

void Function::indentMore(void)
{
    _indentStops++;
}

void Function::indentLess(void)
{
    _indentStops--;
}

ostream& Function::indent(ostream& os) const
{
    const size_t spacesPerStop = 2;

    for (size_t i = 0; i < _indentStops * spacesPerStop; i++)
    {
        os << " ";
    }

    return os; 
}

Function::Function(const size_t deviceIndex,
                   const uint64_t hashCode,
                   const size_t kernelNumber)
    : _deviceIndex(deviceIndex),
      _functionName(makeName(hashCode, kernelNumber)),
      _fp64(false),
      _sampler(NULL),
      _indentStops(0),
      _loopIndexes(0) { }

Function::~Function(void)
{
    delete _sampler;

    for (vector< Variable* >::const_iterator
         it = _arguments.begin();
         it != _arguments.end();
         it++)
    {
        delete *it;
    }

    for (vector< Variable* >::const_iterator
         it = _privates.begin();
         it != _privates.end();
         it++)
    {
        delete *it;
    }
}

string Function::varNameLookup(const Variable* varPtr)
{
    stringstream ss;

    if (_argumentLookup.count(varPtr))
    {
        ss << "a" << _argumentLookup[varPtr];
    }

    if (_privateLookup.count(varPtr))
    {
        ss << "r" << _privateLookup[varPtr];
    }

    return ss.str();
}

string Function::functionName(void) const
{
    return _functionName;
}

bool Function::isFP64(void) const
{
    return _fp64;
}

void Function::addArgument(const uint32_t variable, Image2D* obj)
{
    _typeImage2D.insert(obj);
    addTraceArgument(variable, obj);
    if (! _sampler)
    {
        _sampler = new Sampler;
    }
}

void Function::addArgument(const uint32_t variable, AddrMem* obj)
{
    if (obj->fp64()) _fp64 = true;
    _typeAddrMem.insert(obj);
    addTraceArgument(variable, obj);
}

void Function::addArgument(const uint32_t variable, Integer* obj)
{
    _typeInteger.insert(obj);
    addTraceArgument(variable, obj);
}

void Function::addArgument(const AstVariable* variable, Image2D* obj)
{
    _typeImage2D.insert(obj);
    addSplitArgument(variable, obj);
    if (! _sampler)
    {
        _sampler = new Sampler;
    }
}

void Function::addArgument(const AstVariable* variable, AddrMem* obj)
{
    if (obj->fp64()) _fp64 = true;
    _typeAddrMem.insert(obj);
    addSplitArgument(variable, obj);
}

void Function::addArgument(const AstVariable* variable, Integer* obj)
{
    _typeInteger.insert(obj);
    addSplitArgument(variable, obj);
}

AddrMem* Function::getPrivateVar(const size_t precision,
                                 const size_t vectorLength)
{
    const pair< size_t, size_t > key(precision, vectorLength);

    if (0 == _privateVars.count(key))
    {
        AddrMem* newVar = new AddrMem(precision, vectorLength);
        addPrivate(newVar);
        _privateVars[key] = newVar;
    }

    return _privateVars[key];
}

AddrMem* Function::createPrivate(const uint32_t variable,
                                 const size_t precision,
                                 const size_t vectorLength)
{
    AddrMem* newVar = new AddrMem(precision, vectorLength);
    if (newVar->fp64()) _fp64 = true;

    addPrivate(newVar);

    _tracePrivate[variable] = newVar;
    _privateTrace[newVar] = variable;

    return newVar;
}

AddrMem* Function::createPrivate(const AstVariable* variable,
                                 const size_t precision,
                                 const size_t vectorLength)
{
    AddrMem* newVar = new AddrMem(precision, vectorLength);
    if (newVar->fp64()) _fp64 = true;

    addPrivate(newVar);

    _splitPrivate[variable] = newVar;
    _privateSplit[newVar] = variable;

    return newVar;
}

size_t Function::gatherVariable(const uint32_t varNum,
                                const size_t vectorLength,
                                const size_t width,
                                const size_t height)
{
    return _gathering.gatherVariable(varNum, vectorLength, width, height);
}

size_t Function::gatherVariable(const AstVariable* varPtr,
                                const size_t vectorLength,
                                const size_t width,
                                const size_t height)
{
    return _gathering.gatherVariable(varPtr, vectorLength, width, height);
}

size_t Function::gatherSubscript(const size_t variableNumber,
                                 const size_t N,
                                 const bool xHasIndex,
                                 const bool yHasIndex,
                                 const size_t xOffset,
                                 const size_t yOffset)
{
    return _gathering.gatherSubscript(variableNumber,
                                      N,
                                      xHasIndex,
                                      yHasIndex,
                                      xOffset,
                                      yOffset);
}

Variable* Function::gatherVarFromSubscript(const size_t subNum)
{
    const uint32_t varNum = _gathering.traceVarFromSubscript(subNum);

    if (-1 != varNum)
        return traceVar(varNum);
    else
        return splitVar(_gathering.splitVarFromSubscript(subNum));
}

set< uint32_t > Function::gatherTraceVar(void)
{
    set< uint32_t > s;

    for (size_t i = 0; i < _gathering.numberSubscripts(); i++)
    {
        const uint32_t varNum = _gathering.traceVarFromSubscript(i);

        if (-1 != varNum)
            s.insert(varNum);
    }

    return s;
}

void Function::gatherDecl(Subscript& subObj,
                          const set< uint32_t >& traceSuppressTile)
{
    for (size_t i = 0; i < _gathering.numberSubscripts(); i++)
    {
        subObj.setEligibleGather(
                   _gathering.nFromSubscript(i),
                   _gathering.xHasIndexFromSubscript(i),
                   _gathering.yHasIndexFromSubscript(i),
                   _gathering.xVecOffsetFromSubscript(i),
                   _gathering.yVecOffsetFromSubscript(i) );

        Variable* vArg = gatherVarFromSubscript(i);

        const size_t prec = vArg->precision();
        const size_t vlen = vArg->vectorLength();

        const size_t effVLen = 0 == vlen ? PrecType::vecLength(prec) : vlen;

        const AddrMem vGather(prec, effVLen, CONST);

        const size_t width = _gathering.widthFromSubscript(i);
        const size_t height = _gathering.widthFromSubscript(i);

        subObj.setVariable(width, height, effVLen);

        stringstream ss;

        vGather.declareType(ss);
        vArg->identifierName(ss);
        ss << i << " = ";

        if (0 == vlen)
        {
            const uint32_t varNum = _gathering.traceVarFromSubscript(i);

            const bool suppressTile = -1 == varNum
                                          ? false
                                          : traceSuppressTile.count(varNum);

            switch (prec)
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

            vArg->identifierName(ss);
            ss << ", sampler, (int2)(";
            subObj.widthIdx(ss);
            ss << ", ";
            if (suppressTile) subObj.disableBlocking();
            subObj.heightIdx(ss);
            if (suppressTile) subObj.enableBlocking();
            switch (prec)
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
        }
        else
        {
            vArg->identifierName(ss);
            subObj.arraySub(ss);
        }

        ss << ";";

        _gatherText.push_back(ss.str());

        subObj.unsetEligibleGather();
    }
}

void Function::rngVecType(const size_t prec,
                          const size_t vlen)
{
    switch (prec)
    {
        case (PrecType::UInt32) :
        case (PrecType::Int32) :
        case (PrecType::Float) :
            _random123.prec32();
            break;

        case (PrecType::Double) :
            _random123.prec64();
            break;
    }

    stringstream ss;

    switch (vlen)
    {
        case (1) :
        case (2) :
            _random123.vlen2();
            break;

        case (4) :
            _random123.vlen4();
            break;
    }
}

void Function::rngVariant(const int variant)
{
    switch (variant)
    {
        case (1230) :
            _random123.philox();
            break;

        case (1231) :
            _random123.threefry();
            break;
    }
}

string Function::rngUniform(const size_t prec,
                            const size_t vlen,
                            const int variant,
                            const uint64_t seed,
                            Subscript& tid,
                            const size_t repeatIdx) const
{
    stringstream ss;

    switch (variant)
    {
        case (1230) :
            ss << "philox";
            break;

        case (1231) :
            ss << "threefry";
            break;
    }

    ss << "_uniform_" << prec << "_" << vlen << "(";

    tid.disableSubscriptBrackets();
    tid.arraySub(ss);
    tid.enableSubscriptBrackets();

    ss << ", " << seed;

    if (-1 != repeatIdx)
        ss << " + i" << repeatIdx;

    ss << ")";

    return ss.str();
}

string Function::rngNormal(const size_t prec,
                           const size_t vlen,
                           const int variant,
                           const uint64_t seed,
                           Subscript& tid,
                           const size_t repeatIdx) const
{
    stringstream ss;

    switch (variant)
    {
        case (1230) :
            ss << "philox";
            break;

        case (1231) :
            ss << "threefry";
            break;
    }

    ss << "_normal_" << prec << "_" << vlen << "(";

    tid.disableSubscriptBrackets();
    tid.arraySub(ss);
    tid.enableSubscriptBrackets();

    ss << ", " << seed;

    if (-1 != repeatIdx)
        ss << " + i" << repeatIdx;

    ss << ")";

    return ss.str();
}

const vector< Variable* >& Function::arguments(void) const
{
    return _arguments;
}

uint32_t Function::traceVar(Variable* obj)
{
    if (_varTrace.count(obj))
    {
        return _varTrace[obj];
    }
    else if (_privateTrace.count(obj))
    {
        return _privateTrace[obj];
    }
    else
    {
        return -1;
    }
}

Variable* Function::traceVar(const uint32_t variable)
{
    if (_traceVar.count(variable))
    {
        return _traceVar[variable];
    }
    else if (_tracePrivate.count(variable))
    {
        return _tracePrivate[variable];
    }
    else
    {
        return NULL;
    }
}

const AstVariable* Function::splitVar(Variable* obj)
{
    if (_varSplit.count(obj))
    {
        return _varSplit[obj];
    }
    else if (_privateSplit.count(obj))
    {
        return _privateSplit[obj];
    }
    else
    {
        return NULL;
    }
}

Variable* Function::splitVar(const AstVariable* variable)
{
    if (_splitVar.count(variable))
    {
        return _splitVar[variable];
    }
    else if (_splitPrivate.count(variable))
    {
        return _splitPrivate[variable];
    }
    else
    {
        return NULL;
    }
}

uint32_t Function::traceVarPrivate(Variable* obj)
{
    if (_privateTrace.count(obj))
    {
        return _privateTrace[obj];
    }
    else
    {
        return -1;
    }
}

Variable* Function::traceVarPrivate(const uint32_t variable)
{
    if (_tracePrivate.count(variable))
    {
        return _tracePrivate[variable];
    }
    else
    {
        return NULL;
    }
}

const AstVariable* Function::splitVarPrivate(Variable* obj)
{
    if (_privateSplit.count(obj))
    {
        return _privateSplit[obj];
    }
    else
    {
        return NULL;
    }
}

Variable* Function::splitVarPrivate(const AstVariable* variable)
{
    if (_splitPrivate.count(variable))
    {
        return _splitPrivate[variable];
    }
    else
    {
        return NULL;
    }
}

bool Function::isPrivate(Variable* obj) const
{
    return _privateTrace.count(obj) || _privateSplit.count(obj);
}

void Function::assignZero(Variable* lhsVar)
{
    assignNumber(lhsVar, 0);
}

void Function::assignZero(Variable* lhsVar,
                          Subscript* lhsSub)
{
    assignNumber(lhsVar, lhsSub, 0);
}

void Function::assignNumber(Variable* lhsVar,
                            const double rhsVal)
{
    assignNumber(lhsVar, NULL, rhsVal);
}

void Function::assignNumber(Variable* lhsVar,
                            Subscript* lhsSub,
                            const double rhsVal)
{
    stringstream ss;

    indent(ss);
    lhsVar->identifierName(ss);
    if (lhsSub)
    {
        lhsSub->arraySub(ss);
        ss << " ";
    }
    ss << "= ";
    lhsVar->convertType(ss);
    ss << "(" << rhsVal << ");";

    _bodyText.push_back(ss.str());
}

void Function::assignVariables(Variable* lhsVar,
                               Variable* rhsVar)
{
    assignVariables(lhsVar, NULL, rhsVar);
}

void Function::assignVariables(Variable* lhsVar,
                               Subscript* lhsSub,
                               Variable* rhsVar)
{
    stringstream ss;

    indent(ss);
    lhsVar->identifierName(ss);
    if (lhsSub)
    {
        lhsSub->arraySub(ss);
        ss << " ";
    }
    ss << "= ";
    rhsVar->identifierName(ss);
    ss << ";";

    _bodyText.push_back(ss.str());
}

void Function::assignStatement(Variable* lhsVar,
                               const string& rhsStmt)
{
    assignStatement(lhsVar, NULL, rhsStmt);
}

void Function::assignStatement(Variable* lhsVar,
                               Subscript* lhsSub,
                               const string& rhsStmt)
{
    stringstream ss;

    indent(ss);
    lhsVar->identifierName(ss);
    if (lhsSub)
    {
        lhsSub->arraySub(ss);
        ss << " ";
    }
    ss << "= " << rhsStmt << " ;";

    _bodyText.push_back(ss.str());
}

void Function::incrementStatement(Variable* lhsVar,
                                  const string& rhsStmt)
{
    incrementStatement(lhsVar, NULL, rhsStmt);
}

void Function::incrementStatement(Variable* lhsVar,
                                  Subscript* lhsSub,
                                  const string& rhsStmt)
{
    stringstream ss;

    indent(ss);
    lhsVar->identifierName(ss);
    if (lhsSub)
    {
        lhsSub->arraySub(ss);
        ss << " ";
    }
    ss << "+= " << rhsStmt << " ;";

    _bodyText.push_back(ss.str());
}

void Function::barrier(void)
{
    stringstream ss;

    indent(ss);

    // Generated kernels use global memory and images only. There is no use
    // of local memory at all (for now).
    ss << "barrier(CLK_GLOBAL_MEM_FENCE) ;";

    _bodyText.push_back(ss.str());
}

size_t Function::loopIndexes(void) const
{
    return _loopIndexes;
}

size_t Function::loopBegin(const size_t startIndex, const size_t upperLimit)
{
    stringstream ss;

    indent(ss);
    ss << "for (int i"
       << _loopIndexes << " = " << startIndex << "; i"
       << _loopIndexes << " < " << upperLimit << "; i"
       << _loopIndexes << "++)";

    _bodyText.push_back(ss.str());

    ss.str(string());

    indent(ss);
    ss << "{";

    _bodyText.push_back(ss.str());
    
    indentMore();

    return _loopIndexes++;
}

void Function::loopEnd(void)
{
    _loopIndexes--;

    indentLess();

    stringstream ss;

    indent(ss);
    ss << "}";

    _bodyText.push_back(ss.str());
}

void Function::print(vector< string >& outText)
{
    stringstream ss;

    if (_fp64)
    {
        ss << OCLhacks::singleton().pragmaFP64(_deviceIndex)
           << endl;
        outText.push_back(ss.str());
        ss.str(string());
    }

    // RNG
    _random123.print(ss);

    ss << KERNEL << "void " << _functionName << "(" << endl;
    outText.push_back(ss.str());
    ss.str(string());

    indentMore();

    // arguments of function
    for (size_t i = 0; i < _arguments.size(); i++)
    {
        indent(ss);

        _arguments[i]->declareType(ss);
        _arguments[i]->identifierName(ss);

        if (i < _arguments.size() - 1)
        {
            ss << ",";
        }
        else
        {
            ss << ")";
        }
        ss << endl;

        outText.push_back(ss.str());
        ss.str(string());
    }

    indentLess();

    ss << "{" << endl;
    outText.push_back(ss.str());
    ss.str(string());

    indentMore();

    // image sampler
    if (_sampler)
    {
        indent(ss);
        _sampler->declareType(ss);
        ss << "sampler = CLK_FILTER_NEAREST | "
                        "CLK_NORMALIZED_COORDS_FALSE | "
                        "CLK_ADDRESS_NONE;" << endl;
        outText.push_back(ss.str());
        ss.str(string());
    }

    // gather variables
    for (vector< string >::const_iterator
         it = _gatherText.begin();
         it != _gatherText.end();
         it++)
    {
        indent(ss) << (*it) << endl;
        outText.push_back(ss.str());
        ss.str(string());
    }

    // private variables declared at top function body
    for (vector< Variable* >::const_iterator
         it = _privates.begin();
         it != _privates.end();
         it++)
    {
        indent(ss);

        (*it)->declareType(ss);
        (*it)->identifierName(ss);

        ss << ";" << endl;

        outText.push_back(ss.str());
        ss.str(string());
    }

    // body of function
    for (vector< string >::const_iterator
         it = _bodyText.begin();
         it != _bodyText.end();
         it++)
    {
        indent(ss) << (*it) << endl;
        outText.push_back(ss.str());
        ss.str(string());
    }

    indentLess();

    ss << "}" << endl;
    outText.push_back(ss.str());
    ss.str(string());
}

}; // namespace chai_internal
