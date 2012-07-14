include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	BackMem.o \
	ClientTrace.o \
	FrontMem.o \
	SingleNut.o \
	SingleTrace.o \
	VectorNut.o \
	VectorTrace.o

INTERNAL_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp) \
	MemCallback.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
