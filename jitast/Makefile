include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	AstAccum.o \
	AstArrayMem.o \
	AstCond.o \
	AstConvert.o \
	AstDotprod.o \
	AstExtension.o \
	AstFun1.o \
	AstFun2.o \
	AstFun3.o \
	AstGather.o \
	AstIdxdata.o \
	AstLitdata.o \
	AstMakedata.o \
	AstMatmulBase.o \
	AstMatmulMM.o \
	AstMatmulMV.o \
	AstMatmulVM.o \
	AstMatmulVV.o \
	AstOpenCL.o \
	AstReadout.o \
	AstRNGnormal.o \
	AstRNGuniform.o \
	AstScalar.o \
	AstTranspose.o \
	AstVariable.o \
	BaseAst.o \
	BasePrinter.o \
	BaseTrans.o \
	TransAccum.o \
	TransCond.o \
	TransConvert.o \
	TransDotprod.o \
	TransFun1.o \
	TransFun2.o \
	TransFun3.o \
	TransGather.o \
	TransIdxdata.o \
	TransLitdata.o \
	TransMakedata.o \
	TransMatmul.o \
	TransMatmulG.o \
	TransOpenCL.o \
	TransReadout.o \
	TransRNGnormal.o \
	TransRNGuniform.o \
	TransScalar.o \
	TransTranspose.o

INTERNAL_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp) \
	VisitAst.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
