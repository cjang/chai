// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include "KernelParams.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// PackedCalc (exogenous)

Packing::Packing(void)
    : _numCalc(1),
      _dataAcrossTraces(DIFFERENT_DATA),
      _changed(false) { }

void Packing::setPacking(const size_t numCalc)
{
    if (numCalc != _numCalc) _changed = true;
    _numCalc = numCalc;
}

void Packing::setDifferentData(void)
{
    // data across traces doesn't have meaning if there is only one trace
    if (1 != packing())
    {
        if (DIFFERENT_DATA != _dataAcrossTraces)
            _changed = true;

        _dataAcrossTraces = DIFFERENT_DATA;
    }
}

void Packing::setSameDataMatrixA(void)
{
    // data across traces doesn't have meaning if there is only one trace
    if (1 != packing())
    {
        if (SAME_DATA_MATRIX_A != _dataAcrossTraces)
            _changed = true;

        _dataAcrossTraces = SAME_DATA_MATRIX_A;
    }
}

void Packing::setSameDataMatrixB(void)
{
    // data across traces doesn't have meaning if there is only one trace
    if (1 != packing())
    {
        if (SAME_DATA_MATRIX_B != _dataAcrossTraces)
            _changed = true;

        _dataAcrossTraces = SAME_DATA_MATRIX_B;
    }
}

bool Packing::packingChanged(void) const
{
    return _changed;
}

size_t Packing::packing(void) const
{
    return _numCalc;
}

bool Packing::getDifferentData(void) const
{
    return DIFFERENT_DATA == _dataAcrossTraces;
}

bool Packing::getSameDataMatrixA(void) const
{
    return SAME_DATA_MATRIX_A == _dataAcrossTraces;
}

bool Packing::getSameDataMatrixB(void) const
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
    : _sizeofA(0),
      _sizeofB(0),
      _sizeofC(0),
      _changed(false) { }

void Precision::setPrecision(const size_t A,
                             const size_t B,
                             const size_t C)
{
    _changed = (A != _sizeofA) || (B != _sizeofB) || (C != _sizeofC);
    _sizeofA = A;
    _sizeofB = B;
    _sizeofC = C;
}

bool Precision::precisionChanged(void) const
{
    return _changed;
}

size_t Precision::precisionA(void) const
{
    return _sizeofA;
}

size_t Precision::precisionB(void) const
{
    return _sizeofB;
}

size_t Precision::precisionC(void) const
{
    return _sizeofC;
}

////////////////////////////////////////
// Dimensions (exogenous)

Dimensions::Dimensions(void)
    : _dimM(0),
      _dimN(0),
      _dimK(0),
      _changed(false) { }

void Dimensions::setDimensions(const size_t M,
                               const size_t N)
{
    _changed = (M != _dimM) || (N != _dimN);
    _dimM = M;
    _dimN = N;
}

void Dimensions::setDimensions(const size_t M,
                               const size_t N,
                               const size_t K)
{
    _changed = (M != _dimM) || (N != _dimN) || (K != _dimK);
    _dimM = M;
    _dimN = N;
    _dimK = K;
}

bool Dimensions::dimensionChanged(void) const
{
    return _changed;
}

size_t Dimensions::dimensionM(void) const { return _dimM; }
size_t Dimensions::dimensionN(void) const { return _dimN; }
size_t Dimensions::dimensionK(void) const { return _dimK; }

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
    : _dimHeight(0),
      _dimWidth(0),
      globalRow(func_string<size_t>("get_global_id", 1)),
      globalCol(func_string<size_t>("get_global_id", 0)),
      groupRow(func_string<size_t>("get_group_id", 1)),
      groupCol(func_string<size_t>("get_group_id", 0)),
      localRow(func_string<size_t>("get_local_id", 1)),
      localCol(func_string<size_t>("get_local_id", 0)) { }

void WorkGroup::setWorkGroup(const size_t height, const size_t width)
{
    _dimHeight = height;
    _dimWidth = width;
}

void WorkGroup::setWorkGroup(const size_t sz)
{
    setWorkGroup(sz, sz);
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

size_t WorkGroup::groupHeight(void) const { return _dimHeight; }
size_t WorkGroup::groupWidth(void) const { return _dimWidth; }

size_t WorkGroup::groupSize(void) const
{
    return (_dimWidth == _dimHeight) ? _dimWidth : 0;
}

size_t WorkGroup::localHeight(void) const
{
    return groupHeight() + LOCALMEM_PAD;
}

size_t WorkGroup::localWidth(void) const
{
    return groupWidth() + LOCALMEM_PAD;
}

size_t WorkGroup::localSize(void) const
{
    return groupSize() + LOCALMEM_PAD;
}

////////////////////////////////////////
// InnerBlocking (endogenous)

InnerBlocking::InnerBlocking(void)
    : _blockHeight(0),
      _blockWidth(0) { }

void InnerBlocking::setInnerBlocking(const size_t height, const size_t width)
{
    _blockHeight = height;
    _blockWidth = width;
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

size_t InnerBlocking::blockHeight(void) const { return _blockHeight; }
size_t InnerBlocking::blockWidth(void) const { return _blockWidth; }

////////////////////////////////////////
// VectorLength (endogenous)

VectorLength::VectorLength(void)
    : _vlenA(-1),
      _vlenB(-1),
      _vlenC(-1),
      _changed(false) { }

void VectorLength::setVectorLength(const size_t A,
                                   const size_t B,
                                   const size_t C)
{
    // flag change only when reallocation of memory is necessary
    _changed = (0 == _vlenA && 0 != A) || (0 == A && 0 != _vlenA) ||
               (0 == _vlenB && 0 != B) || (0 == B && 0 != _vlenB) ||
               (0 == _vlenC && 0 != C) || (0 == C && 0 != _vlenC);

    // this forces memory reallocation too often
    //_changed = (A != _vlenA) || (B != _vlenB) || (C != _vlenC);

    _vlenA = A;
    _vlenB = B;
    _vlenC = C;
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

size_t VectorLength::vectorLengthA(void) const
{
    return _vlenA;
}

size_t VectorLength::vectorLengthB(void) const
{
    return _vlenB;
}

size_t VectorLength::vectorLengthC(void) const
{
    return _vlenC;
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
