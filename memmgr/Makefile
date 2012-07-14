include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	MemBase.o \
	MemInterp.o \
	MemInterpAlloc.o \
	MemInterpCallback.o \
	MemTransAccess.o \
	MemTransAlloc.o \
	MemTransCallback.o \
	MemTrans.o \
	MemTransCreate.o \
	MemTransJIT.o \
	MemTransPrivate.o

INTERNAL_HEADER_FILES = \
	MemAlloc.hpp \
	MemBase.hpp \
	MemInterp.hpp \
	MemTrans.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
