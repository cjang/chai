include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	AutoParamsMatmul.o \
	GeneratorBaseMatmul.o \
	GeneratorBaseMatvec.o \
	KernelBaseMatmul.o \
	KernelGeneratorMatmul.o \
	KernelGeneratorMatvec.o

INTERNAL_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp)

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
