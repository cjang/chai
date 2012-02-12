include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	ArrayBuf.o \
	AutoTuneBench.o \
	AutoTuneEM.o \
	AutoTuneMemo.o \
	CodeControlFlow.o \
	CodeFormat.o \
	CodeFunction.o \
	CodeImageSample.o \
	CodeInterfaces.o \
	CodeStatements.o \
	CodeType.o \
	CodeValues.o \
	CodeVars.o \
	EvergreenMatmulBase.o \
	EvergreenMatmulMM.o \
	EvergreenMatmulMV.o \
	KernelBase.o \
	KernelParams.o \
	StandardEM.o

INTERNAL_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp)

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
