// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "KernelParams.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// Batching (exogenous)

Batching::Batching(void)
    : _batchNum(1),
      _dataAcrossTraces(DIFFERENT_DATA),
      _changed(false) { }

void Batching::setBatching(const size_t batchNum)
{
    if (batchNum != _batchNum)
        _changed = true;

    _batchNum = batchNum;
}

void Batching::setDifferentData(void)
{
    // data across traces doesn't have meaning if there is only one trace
    if (1 != batching())
    {
        if (DIFFERENT_DATA != _dataAcrossTraces)
            _changed = true;

        _dataAcrossTraces = DIFFERENT_DATA;
    }
}

void Batching::setSameDataMatrixA(void)
{
    // data across traces doesn't have meaning if there is only one trace
    if (1 != batching())
    {
        if (SAME_DATA_MATRIX_A != _dataAcrossTraces)
            _changed = true;

        _dataAcrossTraces = SAME_DATA_MATRIX_A;
    }
}

void Batching::setSameDataMatrixB(void)
{
    // data across traces doesn't have meaning if there is only one trace
    if (1 != batching())
    {
        if (SAME_DATA_MATRIX_B != _dataAcrossTraces)
            _changed = true;

        _dataAcrossTraces = SAME_DATA_MATRIX_B;
    }
}

bool Batching::batchingChanged(void) const
{
    return _changed;
}

size_t Batching::batching(void) const
{
    return _batchNum;
}

bool Batching::getDifferentData(void) const
{
    return DIFFERENT_DATA == _dataAcrossTraces;
}

bool Batching::getSameDataMatrixA(void) const
{
    return SAME_DATA_MATRIX_A == _dataAcrossTraces;
}

bool Batching::getSameDataMatrixB(void) const
{
    return SAME_DATA_MATRIX_B == _dataAcrossTraces;
}

////////////////////////////////////////
// General (exogenous)

General::General(void)
    : _general(false),
      _changed(false) { }

void General::setGeneral(const bool value)
{
    _changed = value != _general;
    _general = value;
}

bool General::generalChanged(void) const
{
    return _changed;
}

bool General::general(void) const
{
    return _general;
}

////////////////////////////////////////
// Precision (exogenous)

Precision::Precision(void)
    : _precA(0),
      _precB(0),
      _precC(0),
      _changed(false) { }

void Precision::setPrecision(const size_t A,
                             const size_t B,
                             const size_t C)
{
    _changed = (A != _precA) || (B != _precB) || (C != _precC);
    _precA = A;
    _precB = B;
    _precC = C;
}

bool Precision::precisionChanged(void) const
{
    return _changed;
}

size_t Precision::precA(void) const
{
    return _precA;
}

size_t Precision::precB(void) const
{
    return _precB;
}

size_t Precision::precC(void) const
{
    return _precC;
}

////////////////////////////////////////
// Dimensions (exogenous)

Dimensions::Dimensions(void)
    : _M(0),
      _N(0),
      _K(0),
      _changed(false) { }

void Dimensions::setDimensions(const size_t M,
                               const size_t N)
{
    _changed = (M != _M) || (N != _N);
    _M = M;
    _N = N;
}

void Dimensions::setDimensions(const size_t M,
                               const size_t N,
                               const size_t K)
{
    _changed = (M != _M) || (N != _N) || (K != _K);
    _M = M;
    _N = N;
    _K = K;
}

bool Dimensions::dimensionsChanged(void) const
{
    return _changed;
}

size_t Dimensions::M(void) const { return _M; }
size_t Dimensions::N(void) const { return _N; }
size_t Dimensions::K(void) const { return _K; }

////////////////////////////////////////
// DataLayout (exogenous)

DataLayout::DataLayout(void)
    : _transposeA(false),
      _transposeB(false),
      _changed(false) { }

void DataLayout::setDataLayout(const bool A)
{
    _changed = (A != _transposeA);
    _transposeA = A;
}

void DataLayout::setDataLayout(const bool A,
                               const bool B)
{
    _changed = (A != _transposeA) || (B != _transposeB);
    _transposeA = A;
    _transposeB = B;
}

bool DataLayout::layoutChanged(void) const
{
    return _changed;
}

bool DataLayout::transposeA(void) const { return _transposeA; }
bool DataLayout::transposeB(void) const { return _transposeB; }

////////////////////////////////////////
// EndogenousBase

EndogenousBase::EndogenousBase(void)
    : _optimizeThisParam(DO_NOT_OPTIMIZE) { }

void EndogenousBase::doNotOptimize(void)
{
    _optimizeThisParam = DO_NOT_OPTIMIZE;
}

void EndogenousBase::activeOptimize(void)
{
    _optimizeThisParam = ACTIVE;
}

void EndogenousBase::inactiveOptimize(void)
{
    _optimizeThisParam = INACTIVE;
}

void EndogenousBase::toggleActive(void)
{
    switch (_optimizeThisParam)
    {
        case (DO_NOT_OPTIMIZE) :
            break;

        case (ACTIVE) :
            _optimizeThisParam = INACTIVE;
            break;

        case (INACTIVE) :
            _optimizeThisParam = ACTIVE;
            break;
    }
}

bool EndogenousBase::isActive(void) const
{
    return ACTIVE == _optimizeThisParam;
}

////////////////////////////////////////
// WorkGroup (endogenous)

WorkGroup::WorkGroup(void)
    : _H(0),
      _W(0),
      globalRow(func_string<size_t>("get_global_id", 1)),
      globalCol(func_string<size_t>("get_global_id", 0)),
      groupRow(func_string<size_t>("get_group_id", 1)),
      groupCol(func_string<size_t>("get_group_id", 0)),
      localRow(func_string<size_t>("get_local_id", 1)),
      localCol(func_string<size_t>("get_local_id", 0)) { }

void WorkGroup::setWorkGroup(const size_t H,
                             const size_t W)
{
    _H = H;
    _W = W;
}

void WorkGroup::setWorkGroup(const size_t SZ)
{
    setWorkGroup(SZ, SZ);
}

void WorkGroup::doNotOptimizeWorkGroup(void)
{
    doNotOptimize();
}

void WorkGroup::activeOptimizeWorkGroup(void)
{
    activeOptimize();
}

void WorkGroup::inactiveOptimizeWorkGroup(void)
{
    inactiveOptimize();
}

size_t WorkGroup::groupH(void) const { return _H; }
size_t WorkGroup::groupW(void) const { return _W; }

size_t WorkGroup::groupSize(void) const
{
    return (_W == _H) ? _W : 0;
}

size_t WorkGroup::localH(void) const
{
    return groupH() + LOCALMEM_PAD;
}

size_t WorkGroup::localW(void) const
{
    return groupW() + LOCALMEM_PAD;
}

size_t WorkGroup::localSize(void) const
{
    return groupSize() + LOCALMEM_PAD;
}

////////////////////////////////////////
// InnerBlocking (endogenous)

InnerBlocking::InnerBlocking(void)
    : _H(0),
      _W(0) { }

void InnerBlocking::setInnerBlocking(const size_t H,
                                     const size_t W)
{
    _H = H;
    _W = W;
}

void InnerBlocking::doNotOptimizeInnerBlocking(void)
{
    doNotOptimize();
}

void InnerBlocking::activeOptimizeInnerBlocking(void)
{
    activeOptimize();
}

void InnerBlocking::inactiveOptimizeInnerBlocking(void)
{
    inactiveOptimize();
}

size_t InnerBlocking::blockH(void) const { return _H; }
size_t InnerBlocking::blockW(void) const { return _W; }

////////////////////////////////////////
// VectorLength (endogenous)

VectorLength::VectorLength(void)
    : _vecLenA(-1),
      _vecLenB(-1),
      _vecLenC(-1),
      _changed(false) { }

void VectorLength::setVectorLength(const size_t A,
                                   const size_t B,
                                   const size_t C)
{
    // flag change only when reallocation of memory is necessary
    _changed = (0 == _vecLenA && 0 != A) || (0 == A && 0 != _vecLenA) ||
               (0 == _vecLenB && 0 != B) || (0 == B && 0 != _vecLenB) ||
               (0 == _vecLenC && 0 != C) || (0 == C && 0 != _vecLenC);

    // this forces memory reallocation too often
    //_changed = (A != _vecLenA) || (B != _vecLenB) || (C != _vecLenC);

    _vecLenA = A;
    _vecLenB = B;
    _vecLenC = C;
}

void VectorLength::doNotOptimizeVectorLength(void)
{
    doNotOptimize();
}

void VectorLength::activeOptimizeVectorLength(void)
{
    activeOptimize();
}

void VectorLength::inactiveOptimizeVectorLength(void)
{
    inactiveOptimize();
}

bool VectorLength::vectorLengthChanged(void) const
{
    return _changed;
}

size_t VectorLength::vecLengthA(void) const
{
    return _vecLenA;
}

size_t VectorLength::vecLengthB(void) const
{
    return _vecLenB;
}

size_t VectorLength::vecLengthC(void) const
{
    return _vecLenC;
}

////////////////////////////////////////
// AttrAutoVec (endogenous)

AttrAutoVec::AttrAutoVec(void)
    : _useAttrAutoVec(true) { }

void AttrAutoVec::setUseAttrAutoVec(const bool value)
{
    _useAttrAutoVec = value;
}

bool AttrAutoVec::getUseAttrAutoVec(void) const
{
    return _useAttrAutoVec;
}

////////////////////////////////////////
// ExtraParam (pure endogenous)

ExtraParamObserver::ExtraParamObserver(const size_t numberVariations,
                                       ExtraParam& subject)
    : _numberVariations(numberVariations),
      _paramValue(0)
{
    subject.addObserver(this);
}

size_t ExtraParamObserver::numberVariations(void) const
{
    return _numberVariations;
}

size_t ExtraParamObserver::getParam(void) const
{
    return _paramValue;
}

void ExtraParamObserver::setParam(const size_t value)
{
    _paramValue = value;
}

ExtraParam::ExtraParam(void)
    : _extraParam(-1),
      _totalVariations(1) { }

ExtraParam& ExtraParam::getExtraParam(void)
{
    return *this;
}

void ExtraParam::addObserver(ExtraParamObserver* observer)
{
    _observers.push_back(observer);
    _observerParams.push_back(0);

    const size_t observerVariations = observer->numberVariations();
    _totalVariations *= observerVariations;

    _numberVariations.push_back(observerVariations);
}

void ExtraParam::setExtraParam(const size_t value)
{
    _extraParam = value;
    size_t shiftValue = 1;

    for (size_t i = 0; i < _observers.size(); i++)
    {
        if (i > 0)
            shiftValue *= _numberVariations[i - 1];

        const size_t observerValue
            = (value / shiftValue) % _numberVariations[i];

        _observers[i]->setParam(observerValue);
        _observerParams[i] = observerValue;
    }
}

void ExtraParam::doNotOptimizeExtraParam(void)
{
    doNotOptimize();
}

void ExtraParam::activeOptimizeExtraParam(void)
{
    activeOptimize();
}

void ExtraParam::inactiveOptimizeExtraParam(void)
{
    inactiveOptimize();
}

vector<size_t> ExtraParam::extraParamDetail(void) const
{
    return _observerParams;
}

size_t ExtraParam::extraParam(void) const
{
    return _extraParam;
}

size_t ExtraParam::totalVariations(void) const
{
    return _totalVariations;
}

////////////////////////////////////////
// ParamInlineDim (pure endogenous)

ParamInlineDim::ParamInlineDim(ExtraParam& subject)
    : ExtraParamObserver(2, subject) { }

bool ParamInlineDim::inlineDim(void) const
{
    return getParam();
}

////////////////////////////////////////
// ParamLoopOrder (pure endogenous)

ParamLoopOrder::ParamLoopOrder(ExtraParam& subject)
    : ExtraParamObserver(6, subject) { }

size_t ParamLoopOrder::loopOrder(void) const
{
    return getParam();
}

////////////////////////////////////////
// ParamGlobalID (pure endogenous)

ParamGlobalID::ParamGlobalID(ExtraParam& subject)
    : ExtraParamObserver(2, subject) { }

bool ParamGlobalID::globalID(void) const
{
    return getParam();
}

}; // namespace chai_internal
