// Copyright 2011 Chris Jang (fastkor@gmail.com) under The Artistic License 2.0

#include <iostream>

#include "ByteCodes.hpp"
#include "InterpAccum.hpp"
#include "InterpBinop.hpp"
#include "InterpCond.hpp"
#include "InterpConvert.hpp"
#include "InterpDotprod.hpp"
#include "InterpGather.hpp"
#include "InterpIdxdata.hpp"
#include "InterpIsomorph.hpp"
#include "InterpLitdata.hpp"
#include "InterpMakedata.hpp"
#include "InterpMatmul.hpp"
#include "InterpReadout.hpp"
#include "InterpRNG.hpp"
#include "InterpRNGnormal.hpp"
#include "InterpRNGuniform.hpp"
#include "InterpScalar.hpp"
#include "Interpreter.hpp"
#include "MemManager.hpp"
#include "PrintBC.hpp"

using namespace std;

namespace chai_internal {

Interpreter::Interpreter(const size_t deviceCode)
    : DeviceBase(deviceCode),
      _genRNG()
{
    _opDisp.addOp(ByteCodes::mean, new InterpAccum(true));
    _opDisp.addOp(ByteCodes::sum, new InterpAccum(false));

    _opDisp.addOp(ByteCodes::max, new InterpBinop(_MAX_::obj()));
    _opDisp.addOp(ByteCodes::min, new InterpBinop(_MIN_::obj()));
    _opDisp.addOp(ByteCodes::operatorADD, new InterpBinop(_ADD_::obj()));
    _opDisp.addOp(ByteCodes::operatorAND, new InterpBinop(_AND_::obj()));
    _opDisp.addOp(ByteCodes::operatorDIV, new InterpBinop(_DIV_::obj()));
    _opDisp.addOp(ByteCodes::operatorEQ, new InterpBinop(_EQ_::obj()));
    _opDisp.addOp(ByteCodes::operatorGE, new InterpBinop(_GE_::obj()));
    _opDisp.addOp(ByteCodes::operatorGT, new InterpBinop(_GT_::obj()));
    _opDisp.addOp(ByteCodes::operatorLE, new InterpBinop(_LE_::obj()));
    _opDisp.addOp(ByteCodes::operatorLT, new InterpBinop(_LT_::obj()));
    _opDisp.addOp(ByteCodes::operatorMUL, new InterpBinop(_MUL_::obj()));
    _opDisp.addOp(ByteCodes::operatorNE, new InterpBinop(_NE_::obj()));
    _opDisp.addOp(ByteCodes::operatorOR, new InterpBinop(_OR_::obj()));
    _opDisp.addOp(ByteCodes::operatorSUB, new InterpBinop(_SUB_::obj()));

    _opDisp.addOp(ByteCodes::make1_f32, new InterpMakedata);
    _opDisp.addOp(ByteCodes::make1_f64, new InterpMakedata);
    _opDisp.addOp(ByteCodes::make2_f32, new InterpMakedata);
    _opDisp.addOp(ByteCodes::make2_f64, new InterpMakedata);

    _opDisp.addOp(ByteCodes::convert_f32, new InterpConvert(false));
    _opDisp.addOp(ByteCodes::convert_f64, new InterpConvert(true));

    _opDisp.addOp(ByteCodes::read_scalar_f32, new InterpReadout);
    _opDisp.addOp(ByteCodes::read_scalar_f64, new InterpReadout);
    _opDisp.addOp(ByteCodes::read1_f32, new InterpReadout);
    _opDisp.addOp(ByteCodes::read1_f64, new InterpReadout);
    _opDisp.addOp(ByteCodes::read2_f32, new InterpReadout);
    _opDisp.addOp(ByteCodes::read2_f64, new InterpReadout);

    _opDisp.addOp(ByteCodes::cond, new InterpCond);

    _opDisp.addOp(ByteCodes::dot_product, new InterpDotprod);

    _opDisp.addOp(ByteCodes::index1_f32, new InterpIdxdata(false, 1));
    _opDisp.addOp(ByteCodes::index1_f64, new InterpIdxdata(true, 1));
    _opDisp.addOp(ByteCodes::index2_f32, new InterpIdxdata(false, 2));
    _opDisp.addOp(ByteCodes::index2_f64, new InterpIdxdata(true, 2));

    _opDisp.addOp(ByteCodes::abs, new InterpIsomorph(_ABS_::obj()));
    _opDisp.addOp(ByteCodes::exp, new InterpIsomorph(_EXP_::obj()));
    _opDisp.addOp(ByteCodes::negate, new InterpIsomorph(_NEGATE_::obj()));
    _opDisp.addOp(ByteCodes::sqrt, new InterpIsomorph(_SQRT_::obj()));

    const float floatOne = 1;
    const double doubleOne = 1;
    _opDisp.addOp(ByteCodes::ones1_f32, new InterpLitdata(floatOne, 1));
    _opDisp.addOp(ByteCodes::ones1_f64, new InterpLitdata(doubleOne, 1));
    _opDisp.addOp(ByteCodes::ones2_f32, new InterpLitdata(floatOne, 2));
    _opDisp.addOp(ByteCodes::ones2_f64, new InterpLitdata(doubleOne, 2));

    const float floatZero = 0;
    const double doubleZero = 0;
    _opDisp.addOp(ByteCodes::zeros1_f32, new InterpLitdata(floatZero, 1));
    _opDisp.addOp(ByteCodes::zeros1_f64, new InterpLitdata(doubleZero, 1));
    _opDisp.addOp(ByteCodes::zeros2_f32, new InterpLitdata(floatZero, 2));
    _opDisp.addOp(ByteCodes::zeros2_f64, new InterpLitdata(doubleZero, 2));

    _opDisp.addOp(ByteCodes::matmul, new InterpMatmul);

    _opDisp.addOp(ByteCodes::rng_normal_make_f32,
                  new InterpRNGnormal(false, _genRNG));
    _opDisp.addOp(ByteCodes::rng_normal_make_f64,
                 new InterpRNGnormal(true, _genRNG));
    _opDisp.addOp(ByteCodes::rng_uniform_make_f32,
                 new InterpRNGuniform(false, _genRNG));
    _opDisp.addOp(ByteCodes::rng_uniform_make_f64,
                 new InterpRNGuniform(true, _genRNG));

    _opDisp.addOp(ByteCodes::gather1_floor, new InterpGather(1));
    _opDisp.addOp(ByteCodes::gather2_floor, new InterpGather(2));

    _opDisp.addOp(ByteCodes::scalar_f32, new InterpScalar(false));
    _opDisp.addOp(ByteCodes::scalar_f64, new InterpScalar(true));
}

void Interpreter::sub_initDevice(MemManager& mm)
{
    _opDisp.setContext(mm);
}

bool Interpreter::sub_evaluate(VectorTrace& vt)
{
    cerr << "HASHCODE " << vt.hashCode()
         << " NUMTRACES " << vt.numTraces()
         << endl;

    _opDisp.setContext(vt);

    // iterate over statements
    for (size_t i = 0; i < vt.traceLength(); i++)
    {
        // print //

        cerr << vt.getVariable(i) << "."
             << vt.getVersion(i) << "\t";

        PrintBC v(cerr, " ");
        vt.getRHS(i).descend(v);

        if (vt.frontMem().count(v.ptrIndex()))
        {
            cerr << "( ";

            for (size_t j = 0; j < vt.numTraces(); j++)
            {
                cerr << vt.frontMem()[v.ptrIndex()][j] << " ";
            }

            cerr << ")";
        }

        cerr << endl;

        // calculate //

        stack< vector< FrontMem* > > outStack;

        _opDisp.setContext(outStack);

        vt.getRHS(i).ascend(_opDisp);

        if (! outStack.empty())
        {
            // set LHS variable
            vt.vectorNuts()[vt.getVariable(i)]->setNut(vt.getVersion(i),
                                                       outStack.top());

            // need MT safety
            static_cast< FrontMem* >(outStack.top().front())
                ->getBackMem()
                ->needMTSafe();
        }
    }

    // the interpreter always succeeds
    return true;
}

}; // namespace chai_internal
