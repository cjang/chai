// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <sstream>

#include "Function.hpp"
#include "OCLhacks.hpp"
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

void Function::addPrivate(FloatPt* obj)
{
    if (obj->fp64()) _fp64 = true;
    _typeFloatPt.insert(obj);
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
        _privates.push_back(_sampler);
    }
}

void Function::addArgument(const uint32_t variable, FloatPt* obj)
{
    if (obj->fp64()) _fp64 = true;
    _typeFloatPt.insert(obj);
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
        _privates.push_back(_sampler);
    }
}

void Function::addArgument(const AstVariable* variable, FloatPt* obj)
{
    if (obj->fp64()) _fp64 = true;
    _typeFloatPt.insert(obj);
    addSplitArgument(variable, obj);
}

void Function::addArgument(const AstVariable* variable, Integer* obj)
{
    _typeInteger.insert(obj);
    addSplitArgument(variable, obj);
}

FloatPt* Function::getPrivateVar(const size_t precision,
                                 const size_t vectorLength)
{
    const pair< size_t, size_t > key(precision, vectorLength);

    if (0 == _privateVars.count(key))
    {
        FloatPt* newVar = new FloatPt(precision, vectorLength);
        addPrivate(newVar);
        _privateVars[key] = newVar;
    }

    return _privateVars[key];
}

FloatPt* Function::createPrivate(const uint32_t variable,
                                 const size_t precision,
                                 const size_t vectorLength)
{
    FloatPt* newVar = new FloatPt(precision, vectorLength);
    if (newVar->fp64()) _fp64 = true;

    addPrivate(newVar);

    _tracePrivate[variable] = newVar;
    _privateTrace[newVar] = variable;

    return newVar;
}

FloatPt* Function::createPrivate(const AstVariable* variable,
                                 const size_t precision,
                                 const size_t vectorLength)
{
    FloatPt* newVar = new FloatPt(precision, vectorLength);
    if (newVar->fp64()) _fp64 = true;

    addPrivate(newVar);

    _splitPrivate[variable] = newVar;
    _privateSplit[newVar] = variable;

    return newVar;
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
