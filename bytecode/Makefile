include ../Makefile.config
include ../Makefile.common

USE_CFLAGS=$(SHARED_CFLAGS)

LIBRARY_OBJECT_CODE = \
	BC.o \
	EditStak.o \
	HashBC.o \
	HashJIT.o \
	PrintBC.o \
	RefCnt.o

INTERNAL_HEADER_FILES = \
	ByteCodes.hpp \
	EditStak.hpp \
	HashBC.hpp \
	HashJIT.hpp \
	PrintBC.hpp

EXTERNAL_HEADER_FILES = \
	BC.hpp \
	RefCnt.hpp \
	Stak.hpp \
	Visit.hpp

all : preinstall $(LIBRARY_OBJECT_CODE)

preinstall :
	install --mode=644 $(EXTERNAL_HEADER_FILES) $(EXTERNAL_HEADER_DIR)

install : all
	install --mode=644 $(INTERNAL_HEADER_FILES) $(INTERNAL_HEADER_DIR)
	install $(LIBRARY_OBJECT_CODE) $(INTERNAL_OBJECT_DIR)

clean :
	rm -f *.o
