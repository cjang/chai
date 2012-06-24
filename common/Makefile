include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	BackMem.o \
	ClientTrace.o \
	FrontMem.o \
	MemManagerAllocMem.o \
	MemManagerBackMem.o \
	MemManagerBoilerplate.o \
	MemManagerCreateArrayPrivate.o \
	MemManagerCreateArrayPublic.o \
	MemManagerEnqueueKernel.o \
	MemManagerGetArray.o \
	MemManagerScheduler.o \
	MemManagerUnifyMem.o \
	SingleNut.o \
	SingleTrace.o \
	VectorNut.o \
	VectorTrace.o

INTERNAL_HEADER_FILES = \
	BackMem.hpp \
	ClientTrace.hpp \
	FrontMem.hpp \
	MemManager.hpp \
	SingleNut.hpp \
	SingleTrace.hpp \
	VectorNut.hpp \
	VectorTrace.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
