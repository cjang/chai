include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	BaseInterp.o \
	DispatchInterp.o \
	InterpAccum.o \
	InterpCond.o \
	InterpConvert.o \
	InterpDotprod.o \
	InterpFun1.o \
	InterpFun2.o \
	InterpFun3.o \
	InterpGather.o \
	InterpIdxdata.o \
	InterpLitdata.o \
	InterpMakedata.o \
	InterpMatmul.o \
	InterpMatmulBase.o \
	InterpMatmulG.o \
	InterpReadout.o \
	InterpRNG.o \
	InterpRNGnormal.o \
	InterpRNGuniform.o \
	InterpScalar.o \
	InterpTranspose.o

INTERNAL_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp)

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
