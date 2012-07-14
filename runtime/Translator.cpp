// Copyright 2012 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <set>
#include <stdint.h>
#include <string>
#include <vector>

#include "ByteCodes.hpp"
#include "ByteTrace.hpp"
#include "Enqueue.hpp"
#include "EvergreenMatmulBase.hpp"
#include "MakeBCStmt.hpp"
#include "OCLhacks.hpp"
#include "PrecType.hpp"
#include "TransAccum.hpp"
#include "TransCond.hpp"
#include "TransConvert.hpp"
#include "TransDotprod.hpp"
#include "TransFun1.hpp"
#include "TransFun2.hpp"
#include "TransFun3.hpp"
#include "TransGather.hpp"
#include "TransIdxdata.hpp"
#include "Translator.hpp"
#include "TransLitdata.hpp"
#include "TransMakedata.hpp"
#include "TransMatmul.hpp"
#include "TransMatmulG.hpp"
#include "TransOpenCL.hpp"
#include "TransReadout.hpp"
#include "TransRNGnormal.hpp"
#include "TransRNGuniform.hpp"
#include "TransScalar.hpp"
#include "TransTranspose.hpp"
#include "WorkTrace.hpp"

using namespace std;

namespace chai_internal {

////////////////////////////////////////
// JIT controller

void Translator::op(const uint32_t opCode,
                    BaseTrans* opHandler)
{
    _opDisp.addOp( opCode, opHandler );
}

void Translator::internalOps(void)
{
    op(ByteCodes::convert_u32, new TransConvert(PrecType::UInt32));
    op(ByteCodes::convert_i32, new TransConvert(PrecType::Int32));
    op(ByteCodes::convert_f32, new TransConvert(PrecType::Float));
    op(ByteCodes::convert_f64, new TransConvert(PrecType::Double));
    op(ByteCodes::scalar_u32, new TransScalar(PrecType::UInt32));
    op(ByteCodes::scalar_i32, new TransScalar(PrecType::Int32));
    op(ByteCodes::scalar_f32, new TransScalar(PrecType::Float));
    op(ByteCodes::scalar_f64, new TransScalar(PrecType::Double));
}

void Translator::arithmeticOps(void)
{
    op(ByteCodes::operatorUNARYMINUS, new TransFun1("-"));
    op(ByteCodes::operatorADD,        new TransFun2("+", true));
    op(ByteCodes::operatorDIVIDE,     new TransFun2("/", true));
    op(ByteCodes::operatorMULTIPLY,   new TransFun2("*", true));
    op(ByteCodes::operatorSUBTRACT,   new TransFun2("-", true));
    op(ByteCodes::operatorCOMPLEMENT, new TransFun1("~"));
    op(ByteCodes::operatorNOT,        new TransFun1("!"));
    op(ByteCodes::operatorMODULO,     new TransFun2("%", true));
    op(ByteCodes::operatorSHIFTLEFT,  new TransFun2("<<", true));
    op(ByteCodes::operatorSHIFTRIGHT, new TransFun2(">>", true));
    op(ByteCodes::operatorBITWISEAND, new TransFun2("&", true));
    op(ByteCodes::operatorBITWISEOR,  new TransFun2("|", true));
    op(ByteCodes::operatorBITWISEXOR, new TransFun2("^", true));
    op(ByteCodes::operatorCONDEXPR,   new TransCond);
}

void Translator::predicateOps(void)
{
    op(ByteCodes::operatorLOGICALAND,   new TransFun2("&&", true));
    op(ByteCodes::operatorLOGICALOR,    new TransFun2("||", true));
    op(ByteCodes::operatorEQUAL,        new TransFun2("==", true));
    op(ByteCodes::operatorNOTEQUAL,     new TransFun2("!=", true));
    op(ByteCodes::operatorGREATEREQUAL, new TransFun2(">=", true));
    op(ByteCodes::operatorGREATERTHAN,  new TransFun2(">", true));
    op(ByteCodes::operatorLESSEQUAL,    new TransFun2("<=", true));
    op(ByteCodes::operatorLESSTHAN,     new TransFun2("<", true));
}

void Translator::gatherOps(void)
{
    op(ByteCodes::gather1_floor, new TransGather(1));
    op(ByteCodes::gather2_floor, new TransGather(2));
    op(ByteCodes::index1_u32, new TransIdxdata(1, PrecType::UInt32));
    op(ByteCodes::index1_i32, new TransIdxdata(1, PrecType::Int32));
    op(ByteCodes::index1_f32, new TransIdxdata(1, PrecType::Float));
    op(ByteCodes::index1_f64, new TransIdxdata(1, PrecType::Double));
    op(ByteCodes::index2_u32, new TransIdxdata(2, PrecType::UInt32));
    op(ByteCodes::index2_i32, new TransIdxdata(2, PrecType::Int32));
    op(ByteCodes::index2_f32, new TransIdxdata(2, PrecType::Float));
    op(ByteCodes::index2_f64, new TransIdxdata(2, PrecType::Double));
}

void Translator::dataOps(void)
{
    op(ByteCodes::make1_u32, new TransMakedata(PrecType::UInt32));
    op(ByteCodes::make1_i32, new TransMakedata(PrecType::Int32));
    op(ByteCodes::make1_f32, new TransMakedata(PrecType::Float));
    op(ByteCodes::make1_f64, new TransMakedata(PrecType::Double));
    op(ByteCodes::make2_u32, new TransMakedata(PrecType::UInt32));
    op(ByteCodes::make2_i32, new TransMakedata(PrecType::Int32));
    op(ByteCodes::make2_f32, new TransMakedata(PrecType::Float));
    op(ByteCodes::make2_f64, new TransMakedata(PrecType::Double));
    op(ByteCodes::ones1_u32, new TransLitdata(1, static_cast<uint32_t>(1)));
    op(ByteCodes::ones1_i32, new TransLitdata(1, static_cast<int32_t>(1)));
    op(ByteCodes::ones1_f32, new TransLitdata(1, static_cast<float>(1)));
    op(ByteCodes::ones1_f64, new TransLitdata(1, static_cast<double>(1)));
    op(ByteCodes::ones2_u32, new TransLitdata(2, static_cast<uint32_t>(1)));
    op(ByteCodes::ones2_i32, new TransLitdata(2, static_cast<int32_t>(1)));
    op(ByteCodes::ones2_f32, new TransLitdata(2, static_cast<float>(1)));
    op(ByteCodes::ones2_f64, new TransLitdata(2, static_cast<double>(1)));
    op(ByteCodes::zeros1_u32, new TransLitdata(1, static_cast<uint32_t>(0)));
    op(ByteCodes::zeros1_i32, new TransLitdata(1, static_cast<int32_t>(0)));
    op(ByteCodes::zeros1_f32, new TransLitdata(1, static_cast<float>(0)));
    op(ByteCodes::zeros1_f64, new TransLitdata(1, static_cast<double>(0)));
    op(ByteCodes::zeros2_u32, new TransLitdata(2, static_cast<uint32_t>(0)));
    op(ByteCodes::zeros2_i32, new TransLitdata(2, static_cast<int32_t>(0)));
    op(ByteCodes::zeros2_f32, new TransLitdata(2, static_cast<float>(0)));
    op(ByteCodes::zeros2_f64, new TransLitdata(2, static_cast<double>(0)));
}

void Translator::readOps(void)
{
    op(ByteCodes::read_scalar_u32, new TransReadout(0));
    op(ByteCodes::read_scalar_i32, new TransReadout(0));
    op(ByteCodes::read_scalar_f32, new TransReadout(0));
    op(ByteCodes::read_scalar_f64, new TransReadout(0));
    op(ByteCodes::read1_u32, new TransReadout(1));
    op(ByteCodes::read1_i32, new TransReadout(1));
    op(ByteCodes::read1_f32, new TransReadout(1));
    op(ByteCodes::read1_f64, new TransReadout(1));
    op(ByteCodes::read2_u32, new TransReadout(2));
    op(ByteCodes::read2_i32, new TransReadout(2));
    op(ByteCodes::read2_f32, new TransReadout(2));
    op(ByteCodes::read2_f64, new TransReadout(2));
}

void Translator::randomOps(void)
{
    op(ByteCodes::rng_normal_make_f32, new TransRNGnormal(PrecType::Float));
    op(ByteCodes::rng_normal_make_f64, new TransRNGnormal(PrecType::Double));
    op(ByteCodes::rng_uniform_make_u32, new TransRNGuniform(PrecType::UInt32));
    op(ByteCodes::rng_uniform_make_i32, new TransRNGuniform(PrecType::Int32));
    op(ByteCodes::rng_uniform_make_f32, new TransRNGuniform(PrecType::Float));
    op(ByteCodes::rng_uniform_make_f64, new TransRNGuniform(PrecType::Double));
}

void Translator::matmulOps(void)
{
    op(ByteCodes::matmul, new TransMatmul);
    op(ByteCodes::matmulG, new TransMatmulG);
    op(ByteCodes::transpose, new TransTranspose);
}

void Translator::openclOps(void)
{
    op(ByteCodes::kernel_from_opencl, new TransOpenCL);
}

void Translator::reduceOps(void)
{
    op(ByteCodes::dot_product, new TransDotprod);
    op(ByteCodes::mean, new TransAccum(true));
    op(ByteCodes::sum, new TransAccum(false));
}

void Translator::mathOps(void)
{
    op(ByteCodes::acos,      new TransFun1("acos"));
    op(ByteCodes::acosh,     new TransFun1("acosh"));
    op(ByteCodes::acospi,    new TransFun1("acospi"));
    op(ByteCodes::asin,      new TransFun1("asin"));
    op(ByteCodes::asinh,     new TransFun1("asinh"));
    op(ByteCodes::asinpi,    new TransFun1("asinpi"));
    op(ByteCodes::atan,      new TransFun1("atan"));
    op(ByteCodes::atan2,     new TransFun2("atan2", false));
    op(ByteCodes::atanh,     new TransFun1("atanh"));
    op(ByteCodes::atanpi,    new TransFun1("atanpi"));
    op(ByteCodes::atan2pi,   new TransFun2("atan2pi", false));
    op(ByteCodes::cbrt,      new TransFun1("cbrt"));
    op(ByteCodes::ceil,      new TransFun1("ceil"));
    op(ByteCodes::copysign,  new TransFun2("copysign", false));
    op(ByteCodes::cos,       new TransFun1("cos"));
    op(ByteCodes::cosh,      new TransFun1("cosh"));
    op(ByteCodes::cospi,     new TransFun1("cospi"));
    op(ByteCodes::erfc,      new TransFun1("erfc"));
    op(ByteCodes::erf,       new TransFun1("erf"));
    op(ByteCodes::exp,       new TransFun1("exp"));
    op(ByteCodes::exp2,      new TransFun1("exp2"));
    op(ByteCodes::exp10,     new TransFun1("exp10"));
    op(ByteCodes::expm1,     new TransFun1("expm1"));
    op(ByteCodes::fabs,      new TransFun1("fabs"));
    op(ByteCodes::fdim,      new TransFun2("fdim", false));
    op(ByteCodes::floor,     new TransFun1("floor"));
    op(ByteCodes::fma,       new TransFun3("fma"));
    op(ByteCodes::fmax,      new TransFun2("fmax", false));
    op(ByteCodes::fmin,      new TransFun2("fmin", false));
    op(ByteCodes::fmod,      new TransFun2("fmod", false));
    op(ByteCodes::hypot,     new TransFun2("hypot", false));
    op(ByteCodes::ilogb,     new TransFun1("ilogb"));
    op(ByteCodes::ldexp,     new TransFun2("ldexp", false));
    op(ByteCodes::lgamma,    new TransFun1("lgamma"));
    op(ByteCodes::log,       new TransFun1("log"));
    op(ByteCodes::log2,      new TransFun1("log2"));
    op(ByteCodes::log10,     new TransFun1("log10"));
    op(ByteCodes::log1p,     new TransFun1("log1p"));
    op(ByteCodes::logb,      new TransFun1("logb"));
    op(ByteCodes::mad,       new TransFun3("mad"));
    op(ByteCodes::maxmag,    new TransFun2("maxmag", false));
    op(ByteCodes::minmag,    new TransFun2("minmag", false));
    op(ByteCodes::nan,       new TransFun1("nan"));
    op(ByteCodes::nextafter, new TransFun2("nextafter", false));
    op(ByteCodes::pow,       new TransFun2("pow", false));
    op(ByteCodes::pown,      new TransFun2("pown", false));
    op(ByteCodes::powr,      new TransFun2("powr", false));
    op(ByteCodes::remainder, new TransFun2("remainder", false));
    op(ByteCodes::rint,      new TransFun1("rint"));
    op(ByteCodes::rootn,     new TransFun2("rootn", false));
    op(ByteCodes::round,     new TransFun1("round"));
    op(ByteCodes::rsqrt,     new TransFun1("rsqrt"));
    op(ByteCodes::sin,       new TransFun1("sin"));
    op(ByteCodes::sinh,      new TransFun1("sinh"));
    op(ByteCodes::sinpi,     new TransFun1("sinpi"));
    op(ByteCodes::sqrt,      new TransFun1("sqrt"));
    op(ByteCodes::tan,       new TransFun1("tan"));
    op(ByteCodes::tanh,      new TransFun1("tanh"));
    op(ByteCodes::tanpi,     new TransFun1("tanpi"));
    op(ByteCodes::tgamma,    new TransFun1("tgamma"));
    op(ByteCodes::trunc,     new TransFun1("trunc"));
}

void Translator::commonOps(void)
{
    op(ByteCodes::clamp,      new TransFun3("clamp"));
    op(ByteCodes::degrees,    new TransFun1("degrees"));
    op(ByteCodes::max,        new TransFun2("max", false));
    op(ByteCodes::min,        new TransFun2("min", false));
    op(ByteCodes::mix,        new TransFun3("mix"));
    op(ByteCodes::radians,    new TransFun1("radians"));
    op(ByteCodes::step,       new TransFun2("step", false));
    op(ByteCodes::smoothstep, new TransFun3("smoothstep"));
    op(ByteCodes::sign,       new TransFun1("sign"));
}

void Translator::integerOps(void)
{
    op(ByteCodes::abs,      new TransFun1("abs"));
    op(ByteCodes::abs_diff, new TransFun2("abs_diff", false));
    op(ByteCodes::add_sat,  new TransFun2("add_sat", false));
    op(ByteCodes::clz,      new TransFun1("clz"));
    op(ByteCodes::hadd,     new TransFun2("hadd", false));
    op(ByteCodes::mad24,    new TransFun3("mad24"));
    op(ByteCodes::mad_hi,   new TransFun3("mad_hi"));
    op(ByteCodes::mad_sat,  new TransFun3("mad_sat"));
    op(ByteCodes::mul24,    new TransFun2("mul24", false));
    op(ByteCodes::mul_hi,   new TransFun2("mul_hi", false));
    op(ByteCodes::rhadd,    new TransFun2("rhadd", false));
    op(ByteCodes::rotate,   new TransFun2("rotate", false));
    op(ByteCodes::sub_sat,  new TransFun2("sub_sat", false));
}

void Translator::relationalOps(void)
{
    op(ByteCodes::isequal,        new TransFun2("isequal", false));
    op(ByteCodes::isnotequal,     new TransFun2("isnotequal", false));
    op(ByteCodes::isgreater,      new TransFun2("isgreater", false));
    op(ByteCodes::isgreaterequal, new TransFun2("isgreaterequal", false));
    op(ByteCodes::isless,         new TransFun2("isless", false));
    op(ByteCodes::islessequal,    new TransFun2("islessequal", false));
    op(ByteCodes::islessgreater,  new TransFun2("islessgreater", false));
    op(ByteCodes::isfinite,       new TransFun1("isfinite"));
    op(ByteCodes::isinf,          new TransFun1("isinf"));
    op(ByteCodes::isnan,          new TransFun1("isnan"));
    op(ByteCodes::isnormal,       new TransFun1("isnormal"));
    op(ByteCodes::isordered,      new TransFun2("isordered", false));
    op(ByteCodes::isunordered,    new TransFun2("isunordered", false));
    op(ByteCodes::signbit,        new TransFun1("signbit"));
}

Translator::Translator(const size_t deviceCode)
    : DeviceBase(deviceCode),
      _stdEM(NULL),
      _jitMemo()
{
    internalOps();
    arithmeticOps();
    predicateOps();
    gatherOps();
    dataOps();
    readOps();
    randomOps();
    matmulOps();
    openclOps();
    reduceOps();
    mathOps();
    commonOps();
    integerOps();
    relationalOps();
}

Translator::~Translator(void)
{
    delete _stdEM;

    set< JustInTimeMemo* > memoSet;

    for (map< uint64_t, JustInTimeMemo* >::const_iterator
         it = _jitMemo.begin(); it != _jitMemo.end(); it++)
    {
        memoSet.insert((*it).second);
    }

    for (set< JustInTimeMemo* >::const_iterator
         it = memoSet.begin(); it != memoSet.end(); it++)
    {
        delete *it;
    }
}

bool Translator::extendLanguage(const uint32_t opCode,
                                const BaseTrans& opHandler)
{
    BaseTrans* cloneHandler = opHandler.clone();

    if (cloneHandler)
    {
        cloneHandler->setContext(getMemTrans());

        _opDisp.deleteOp(opCode); // release old handler, if any

        op(opCode, cloneHandler); // replace with new handler

        return true;
    }
    else
    {
        return false;
    }
}

void Translator::sub_initDevice(MemTrans& mm)
{
    _opDisp.setContext(mm);

    const size_t deviceIdx = getDeviceNum();
    const size_t numSearch = OCLhacks::singleton().searchTrials(deviceIdx);
    const size_t numTiming = OCLhacks::singleton().timingTrials(deviceIdx);

    _stdEM = new StandardEM(getOclDevice());
    _stdEM->setNumberTrials(numSearch, numTiming);
    _stdEM->setJournalFile(OCLhacks::singleton().journalFile(deviceIdx));
}

bool Translator::sub_evaluate(VectorTrace& vt)
{
    _opDisp.setContext(vt);

    // create bytecode statements from vector trace
    ByteTrace bytecodeTrace(vt);

    // bytecode transformations: lift constants; loop rolling
    bytecodeTrace.optimizeBytecode();

    // generate AST statements
    MakeBCStmt astMaker(_opDisp);
    bytecodeTrace.traverse(astMaker);

    // create statement trace, sort, kernelize, and transforms
    WorkTrace kernelizedTrace(bytecodeTrace);
    kernelizedTrace.reorder();
    kernelizedTrace.together();

    // unique just-in-time memo for each continued trace
    JustInTimeMemo* jitMemo = NULL;
    for (vector< uint64_t >::const_iterator
         it = vt.hashCodeHistory().begin();
         it != vt.hashCodeHistory().end();
         it++)
    {
        const uint64_t hashCode = *it;
        if (0 == _jitMemo.count(hashCode))
        {
            if (NULL == jitMemo)
                jitMemo = new JustInTimeMemo(getMemTrans());

            _jitMemo[hashCode] = jitMemo;
        }
        else
        {
            jitMemo = _jitMemo[hashCode];
        }
    }

    // enqueue
    Enqueue enqueueToDevice(vt, getMemTrans(), *_stdEM, *jitMemo);
    kernelizedTrace.traverse(enqueueToDevice);

    return enqueueToDevice.isOk();
}

}; // namespace chai_internal
