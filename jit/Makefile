include ../Makefile.common

LIBRARY_OBJECT_CODE = \
	AstAccum.o \
	AstArrayMem.o \
	AstBinop.o \
	AstCond.o \
	AstConvert.o \
	AstDotprod.o \
	AstGather.o \
	AstIdxdata.o \
	AstIsomorph.o \
	AstLitdata.o \
	AstMakedata.o \
	AstMatmulMM.o \
	AstMatmulMV.o \
	AstMatmulVM.o \
	AstMatmulVV.o \
	AstReadout.o \
	AstRNGnormal.o \
	AstRNGuniform.o \
	AstScalar.o \
	AstVariable.o \
	BaseAst.o \
	BaseTrans.o \
	BCStmt.o \
	BCStmtCompound.o \
	BCStmtMaker.o \
	BCStmtPrinter.o \
	BCStmtRepeat.o \
	BCStmtSingle.o \
	BCStmtTrace.o \
	DispatchTrans.o \
	TransAccum.o \
	TransBinop.o \
	TransCond.o \
	TransConvert.o \
	TransDotprod.o \
	TransGather.o \
	TransIdxdata.o \
	TransIsomorph.o \
	TransLitdata.o \
	TransMakedata.o \
	TransMatmul.o \
	TransReadout.o \
	TransRNGnormal.o \
	TransRNGuniform.o \
	TransScalar.o \
	VisitBCStmt.o \
	VisitXStmt.o \
	XStmt.o \
	XStmtCompound.o \
	XStmtCreateData.o \
	XStmtEnqueue.o \
	XStmtGather.o \
	XStmtIdSpace.o \
	XStmtIndex.o \
	XStmtLiteral.o \
	XStmtMaker.o \
	XStmtMatmul.o \
	XStmtPrinter.o \
	XStmtReadData.o \
	XStmtReduce.o \
	XStmtRepeat.o \
	XStmtRNGrand.o \
	XStmtRNGseed.o \
	XStmtSendData.o \
	XStmtSingle.o \
	XStmtTogether.o \
	XStmtTrace.o

LIBRARY_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp)

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(LIBRARY_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
