include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS) -D__STDC_LIMIT_MACROS

LIBRARY_OBJECT_CODE = \
	Logger.o \
	PrecType.o \
	SimpleFuns.o \
	TEA.o

INTERNAL_HEADER_FILES = \
	$(LIBRARY_OBJECT_CODE:.o=.hpp) \
	MemalignSTL.hpp \
	UtilFuns.hpp

all : $(LIBRARY_OBJECT_CODE)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
