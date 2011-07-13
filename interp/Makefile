include ../Makefile.common

LIBRARY_OBJECT_CODE = \
	BaseInterp.o \
	DispatchInterp.o \
	InterpAccum.o \
	InterpBinop.o \
	InterpCond.o \
	InterpConvert.o \
	InterpDotprod.o \
	InterpGather.o \
	InterpIdxdata.o \
	InterpIsomorph.o \
	InterpLitdata.o \
	InterpMakedata.o \
	InterpMatmul.o \
	InterpReadout.o \
	InterpRNG.o \
	InterpRNGnormal.o \
	InterpRNGuniform.o \
	InterpScalar.o

LIBRARY_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp)

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(LIBRARY_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
