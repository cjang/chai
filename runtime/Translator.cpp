// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>

#include "BCStmtMaker.hpp"
#include "BCStmtPrinter.hpp"
#include "BCStmtTrace.hpp"
#include "ByteCodes.hpp"
#include "TransAccum.hpp"
#include "TransBinop.hpp"
#include "TransCond.hpp"
#include "TransConvert.hpp"
#include "TransDotprod.hpp"
#include "TransGather.hpp"
#include "TransIdxdata.hpp"
#include "TransIsomorph.hpp"
#include "Translator.hpp"
#include "TransLitdata.hpp"
#include "TransMakedata.hpp"
#include "TransMatmul.hpp"
#include "TransReadout.hpp"
#include "TransRNGnormal.hpp"
#include "TransRNGuniform.hpp"
#include "TransScalar.hpp"
#include "XStmtEnqueue.hpp"
#include "XStmtTrace.hpp"

using namespace std;

namespace chai_internal {

Translator::Translator(const size_t deviceCode)
    : DeviceBase(deviceCode)
{
    _opDisp.addOp(ByteCodes::mean, new TransAccum(true));
    _opDisp.addOp(ByteCodes::sum, new TransAccum(false));

    _opDisp.addOp(ByteCodes::max, new TransBinop(_MAX_::obj()));
    _opDisp.addOp(ByteCodes::min, new TransBinop(_MIN_::obj()));
    _opDisp.addOp(ByteCodes::operatorADD, new TransBinop(_ADD_::obj()));
    _opDisp.addOp(ByteCodes::operatorAND, new TransBinop(_AND_::obj()));
    _opDisp.addOp(ByteCodes::operatorDIV, new TransBinop(_DIV_::obj()));
    _opDisp.addOp(ByteCodes::operatorEQ, new TransBinop(_EQ_::obj()));
    _opDisp.addOp(ByteCodes::operatorGE, new TransBinop(_GE_::obj()));
    _opDisp.addOp(ByteCodes::operatorGT, new TransBinop(_GT_::obj()));
    _opDisp.addOp(ByteCodes::operatorLE, new TransBinop(_LE_::obj()));
    _opDisp.addOp(ByteCodes::operatorLT, new TransBinop(_LT_::obj()));
    _opDisp.addOp(ByteCodes::operatorMUL, new TransBinop(_MUL_::obj()));
    _opDisp.addOp(ByteCodes::operatorNE, new TransBinop(_NE_::obj()));
    _opDisp.addOp(ByteCodes::operatorOR, new TransBinop(_OR_::obj()));
    _opDisp.addOp(ByteCodes::operatorSUB, new TransBinop(_SUB_::obj()));

    _opDisp.addOp(ByteCodes::make1_f32, new TransMakedata(false));
    _opDisp.addOp(ByteCodes::make1_f64, new TransMakedata(true));
    _opDisp.addOp(ByteCodes::make2_f32, new TransMakedata(false));
    _opDisp.addOp(ByteCodes::make2_f64, new TransMakedata(true));

    _opDisp.addOp(ByteCodes::convert_f32, new TransConvert(false));
    _opDisp.addOp(ByteCodes::convert_f64, new TransConvert(true));

    _opDisp.addOp(ByteCodes::read_scalar_f32, new TransReadout(false));
    _opDisp.addOp(ByteCodes::read_scalar_f64, new TransReadout(true));
    _opDisp.addOp(ByteCodes::read1_f32, new TransReadout(false));
    _opDisp.addOp(ByteCodes::read1_f64, new TransReadout(true));
    _opDisp.addOp(ByteCodes::read2_f32, new TransReadout(false));
    _opDisp.addOp(ByteCodes::read2_f64, new TransReadout(true));

    _opDisp.addOp(ByteCodes::cond, new TransCond);

    _opDisp.addOp(ByteCodes::dot_product, new TransDotprod);

    _opDisp.addOp(ByteCodes::index1_f32, new TransIdxdata(1, false));
    _opDisp.addOp(ByteCodes::index1_f64, new TransIdxdata(1, true));
    _opDisp.addOp(ByteCodes::index2_f32, new TransIdxdata(2, false));
    _opDisp.addOp(ByteCodes::index2_f64, new TransIdxdata(2, true));

    _opDisp.addOp(ByteCodes::abs, new TransIsomorph(_ABS_::obj()));
    _opDisp.addOp(ByteCodes::exp, new TransIsomorph(_EXP_::obj()));
    _opDisp.addOp(ByteCodes::negate, new TransIsomorph(_NEGATE_::obj()));
    _opDisp.addOp(ByteCodes::sqrt, new TransIsomorph(_SQRT_::obj()));

    const float floatOne = 1;
    const double doubleOne = 1;
    _opDisp.addOp(ByteCodes::ones1_f32, new TransLitdata(1, floatOne));
    _opDisp.addOp(ByteCodes::ones1_f64, new TransLitdata(1, doubleOne));
    _opDisp.addOp(ByteCodes::ones2_f32, new TransLitdata(2, floatOne));
    _opDisp.addOp(ByteCodes::ones2_f64, new TransLitdata(2, doubleOne));

    const float floatZero = 0;
    const double doubleZero = 0;
    _opDisp.addOp(ByteCodes::zeros1_f32, new TransLitdata(1, floatZero));
    _opDisp.addOp(ByteCodes::zeros1_f64, new TransLitdata(1, doubleZero));
    _opDisp.addOp(ByteCodes::zeros2_f32, new TransLitdata(2, floatZero));
    _opDisp.addOp(ByteCodes::zeros2_f64, new TransLitdata(2, doubleZero));

    _opDisp.addOp(ByteCodes::matmul, new TransMatmul);

    _opDisp.addOp(ByteCodes::rng_normal_make_f32, new TransRNGnormal(false));
    _opDisp.addOp(ByteCodes::rng_normal_make_f64, new TransRNGnormal(true));
    _opDisp.addOp(ByteCodes::rng_uniform_make_f32, new TransRNGuniform(false));
    _opDisp.addOp(ByteCodes::rng_uniform_make_f64, new TransRNGuniform(true));

    _opDisp.addOp(ByteCodes::gather1_floor, new TransGather(1));
    _opDisp.addOp(ByteCodes::gather2_floor, new TransGather(2));

    _opDisp.addOp(ByteCodes::scalar_f32, new TransScalar(false));
    _opDisp.addOp(ByteCodes::scalar_f64, new TransScalar(true));
}

void Translator::sub_initDevice(MemManager& mm)
{
    _opDisp.setContext(mm);
}

bool Translator::sub_evaluate(VectorTrace& vt)
{
    _opDisp.setContext(vt);

    cerr << "JIT translator device " << getDeviceCode() << endl;

    BCStmtTrace bcsTrace(vt);

    // bytecode code transformations
    bcsTrace.optimizeBytecode();

    // generate boxed statements
    BCStmtMaker bcsMaker(_opDisp);
    bcsTrace.traverse(bcsMaker);

    // working trace
    XStmtTrace xsTrace(bcsTrace);

    // perform reorder and together
    xsTrace.reorder();
    xsTrace.together();

    // enqueue
    XStmtEnqueue xsEnqueue(vt, getMemManager(), cerr);
    xsTrace.traverse(xsEnqueue);
    cerr << endl;

    // not supporting JIT devices yet
    return xsEnqueue.isOk();
}

}; // namespace chai_internal
