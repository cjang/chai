include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	EnqueueAST.o \
	EnqueueBase.o \
	EnqueueBarrier.o \
	EnqueueCompound.o \
	EnqueueCreateData.o \
	EnqueueExtension.o \
	EnqueueExtensionAuto.o \
	EnqueueGatherAuto.o \
	EnqueueIdSpace.o \
	EnqueueIndex.o \
	EnqueueLiteral.o \
	EnqueueMatmul.o \
	EnqueueMatmulAuto.o \
	EnqueueOpenCL.o \
	EnqueueReadData.o \
	EnqueueReduce.o \
	EnqueueRepeat.o \
	EnqueueSendData.o \
	EnqueueSingle.o \
	JustInTimeMemo.o

INTERNAL_HEADER_FILES = \
	Enqueue.hpp \
	JustInTimeMemo.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
