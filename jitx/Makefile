include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	ElideInternal.o \
	EnqueueTraceAST.o \
	EnqueueTraceBase.o \
	EnqueueTraceBarrier.o \
	EnqueueTraceCompound.o \
	EnqueueTraceCreateData.o \
	EnqueueTraceExtension.o \
	EnqueueTraceExtensionAuto.o \
	EnqueueTraceGatherAuto.o \
	EnqueueTraceIdSpace.o \
	EnqueueTraceIndex.o \
	EnqueueTraceLiteral.o \
	EnqueueTraceMatmul.o \
	EnqueueTraceMatmulAuto.o \
	EnqueueTraceOpenCL.o \
	EnqueueTraceReadData.o \
	EnqueueTraceReduce.o \
	EnqueueTraceRepeat.o \
	EnqueueTraceSendData.o \
	EnqueueTraceSingle.o \
	GroupTogether.o \
	JustInTimeMemo.o \
	MakeStmt.o \
	PrintTrace.o \
	StmtIdSpace.o \
	TrimFat.o \
	UseRegister.o \
	WorkTrace.o


INTERNAL_HEADER_FILES = \
	ElideInternal.hpp \
	EnqueueTrace.hpp \
	GroupTogether.hpp \
	JustInTimeMemo.hpp \
	MakeStmt.hpp \
	PrintTrace.hpp \
	StmtIdSpace.hpp \
	TrimFat.hpp \
	UseRegister.hpp \
	WorkTrace.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
